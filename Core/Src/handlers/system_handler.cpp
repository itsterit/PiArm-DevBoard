#include <main.h>
#define COIL_CURRENT_FAULT_DELAY (1000)
#define CHECK_SYSTEM_TIMEOUT (250)

extern "C" void NMI_Handler(void)
{
    asm("NOP");
    NVIC_SystemReset();
}

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
uint16_t volatile dc_startup = 0;
uint16_t volatile check_system_timeout = 0;
extern "C" void SysTick_Handler(void)
{
    if (check_system_timeout++ >= CHECK_SYSTEM_TIMEOUT)
    {
        check_system_timeout = 0;
        ADC_INJ_START(ADC2);
    }

    if (dc_startup)
    {
        if (dc_startup < 500)
            led_pin.set();

        if (--dc_startup == 0)
        {
            SysTick->CTRL = 0x00;
            SysTick->VAL = 0x00;
            SysTick->LOAD = 0x00;
            NVIC_DisableIRQ(SysTick_IRQn);

            if (system_monitor_handler(usInputRegisters[INPUT_REG_REF_VOLTAGE], usInputRegisters[INPUT_REG_BAT_VOLTAGE], usInputRegisters[INPUT_REG_DC_VOLTAGE]) != SYSTEM_OK)
            {
                led_pin.reset();
                dc_enable.reset();
                NVIC_SystemReset();
            }
        }
    }

    if (cur_fault_delay)
    {
        if (--cur_fault_delay == 0)
        {
            led_pin.set();
            set_timer_config();
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
#if INVERT_GENERATOR_SIGNAL
        GPIOB->BSRR = (GPIO_BSRR_BS5_Msk);
#else
        GPIOB->BRR = (GPIO_BRR_BR5_Msk);
#endif
        EXTI->PR = EXTI->PR;
        cur_fault_delay = COIL_CURRENT_FAULT_DELAY;
        led_pin.reset();
    }
    __enable_irq();
}

extern "C" void ADC1_2_IRQHandler(void)
{
    if (ADC2->SR & ADC_SR_AWD_Msk)
    {
        __disable_irq();
        GPIOB->CRL &= ~(GPIO_CRL_CNF5_Msk);
        GPIOB->CRL |= (GPIO_CRL_MODE5_Msk);
#if INVERT_GENERATOR_SIGNAL
        GPIOB->BSRR = (GPIO_BSRR_BS5_Msk);
#else
        GPIOB->BRR = (GPIO_BRR_BR5_Msk);
#endif
        ADC2->SR = ~ADC2->SR;
        cur_fault_delay = COIL_CURRENT_FAULT_DELAY;
        led_pin.reset();
        __enable_irq();
    }
    else
    {
        ADC1->SR = ~ADC1->SR;
        GPIOB->BSRR = (0b01 << 11U);
        GPIOB->BRR = (0b01 << 11U);
    }
}

#ifndef __GNUC__
#warning "Please check the compiler you are using!"
#endif

#if USB_BUFFER_SIZE > DMA_MAX_FRAME_SIZE
#warning "Transmission buffer size determination is incorrect!"
#undef USB_BUFFER_SIZE
#define USB_BUFFER_SIZE (0xFF)
#endif
