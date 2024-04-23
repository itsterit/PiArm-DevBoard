#include <main.h>

#ifndef __GNUC__
#warning "Please check the compiler you are using!"
#endif /* #ifndef __GNUC__ */

#if USB_BUFFER_SIZE > DMA_MAX_FRAME_SIZE
#warning "Transmission buffer size determination is incorrect!"
#undef USB_BUFFER_SIZE
#define USB_BUFFER_SIZE (0xFF)
#endif /* USB_BUFFER_SIZE > DMA_MAX_FRAME_SIZE */

extern "C" void HardFault_Handler(void)
{
    __asm volatile(
        /* Кол-во итерация для извлечения LR и PC из стека */
        "LDR R0, =7				\n"
        /* Тут должны дойти до LR регистра */
        "read_stack:			\n"
        "SUBS R0, R0, #1 		\n"
        "POP {r1}				\n"
        /* Проверяем что мы дошли до LR регистра в стеке */
        "CBZ R0, get_stack_PC	\n"
        "B read_stack			\n"
        /* Тут извлекаем PC регистр */
        "get_stack_PC:			\n"
        "POP {r0}				\n"
        /* Сохраняем полученные данные в noinit  */
        "LDR r4, =0x20004FF6 	\n" // PC
        "STR r0, [r4]			\n"
        "LDR r4, =0x20004FFA	\n" // LR
        "STR r1, [r4]			\n");
    NVIC_SystemReset();
}

/**
 * @brief   Прерывание системного таймера
 * @details Установлен наименьший приоритет
 * @note    В большинстве своем для работы в бесконечном цикле
 *          проверяются статус флаги.
 */
uint16_t cnt = 0;
extern "C" void SysTick_Handler(void)
{
    if (cnt++ >= 2500)
    {
        cnt = 0;
        ADC1->CR2 |= (ADC_CR2_JSWSTART_Msk);
    }

    if (cur_fault_delay)
    {
        if (!(--cur_fault_delay))
        {
            led_pin.reset();
            set_timer_config();
        }
    }

    if (reboot_delay)
    {
        if (!(reboot_delay % 100))
        {
            if (led_pin.get_level())
            {
                led_pin.reset();
            }
            else
            {
                led_pin.set();
            }
        }
        if (!(--reboot_delay))
        {
            NVIC_SystemReset();
        }
    }
}

/**
 * @brief   Сработала защита по току катушки
 * @details Отключаем генерацию частоты катушки
 * @note    Для надежности переводим ногу проца в режим GPIO и после отработки тайминга
 *          защиты возвращаем в исходное состояние.
 * @warning В боевой плате применен драйвер ключа инвертирующий входной сигнал!!!
 * @warning Прерывание с самым высоким приоритетом.
 */
extern "C" void EXTI15_10_IRQHandler(void)
{
    __disable_irq();
    {
        GPIOB->CRL &= ~(GPIO_CRL_CNF5_Msk);
        GPIOB->CRL |= (GPIO_CRL_MODE5_Msk);
        GPIOB->BSRR = (GPIO_BSRR_BS5_Msk);

        while (cur_fault.get_level() == 0)
            asm("NOP");

        EXTI->PR = EXTI->PR;
        cur_fault_delay = 0xFF;
        led_pin.set();
    }
    __enable_irq();
}