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

    while (1)
    {
        asm("NOP");
    }
    // NVIC_SystemReset();
}

bool led_out = 0;
extern "C" void SysTick_Handler(void)
{
    if (led_out)
    {
        led_out = 0;
        led_pin.set();
    }
    else
    {
        led_out = 1;
        led_pin.reset();
    }
}

/**
 * @brief   Сработала защита по току катушки
 * @details Отключаем генерацию частоты катушки
 * @note    Для надежности переводим ногу проца в режим GPIO и после отработки тайминга
 *          защиты возвращаем в исходное состояние.
 * @warning В боевой плате применен драйвер ключа инвертирующий входной сигнал!!!
 */
extern "C" void EXTI15_10_IRQHandler(void)
{

    GPIOB->CRL &= ~(GPIO_CRL_CNF5_Msk);
    GPIOB->CRL |= (GPIO_CRL_MODE5_Msk);
#if INVERT_GENERATOR_SIGNAL
    GPIOB->BSRR |= (GPIO_BSRR_BS5_Msk);
#else
    GPIOB->BSRR |= (GPIO_BSRR_BR5_Msk);
#endif

    EXTI->PR = EXTI->PR;
}