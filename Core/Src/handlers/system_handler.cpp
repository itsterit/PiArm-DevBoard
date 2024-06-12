#include <main.h>
#include "ModBus/mbcrc/mbcrc.h"
#define COIL_CURRENT_FAULT_DELAY (1000)
#define CHECK_SYSTEM_TIMEOUT (100)
#define UPDATE_TIMEOUT (2000)
// Параметры
#define ALPHA_SMOOTH_VALUE (0.07)
#define COIL_CURRENT_SMOOTH_VALUE (0.2)
#define COIL_CURRENT_RES (0.2)

extern "C" void NMI_Handler(void)
{
    asm("NOP");
    NVIC_SystemReset();
}

extern "C" void HardFault_Handler(void)
{
    /* Кол-во итерация для извлечения LR и PC из стека */
    asm("LDR R0, =7				\n");
    /* Тут должны дойти до LR регистра */
    asm("read_stack:			\n");
    asm("SUBS R0, R0, #1 		\n");
    asm("POP {r1}				\n");
    /* Проверяем что мы дошли до LR регистра в стеке */
    asm("CBZ R0, get_stack_PC	\n");
    asm("B read_stack			\n");
    /* Тут извлекаем PC регистр */
    asm("get_stack_PC:			\n");
    asm("POP {r0}				\n");
    /* Сохраняем полученные данные в noinit  */
    asm("LDR r4, =0x20004FF6 	\n");
    asm("STR r0, [r4]			\n");
    asm("LDR r4, =0x20004FFA	\n");
    asm("STR r1, [r4]			\n");
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
uint16_t volatile update_signal_timeout = 0;
extern "C" void SysTick_Handler(void)
{
    if (check_system_timeout++ >= CHECK_SYSTEM_TIMEOUT)
    {
        check_system_timeout = 0;
        ADC_INJ_START(ADC1);
    }

    if (update_signal_timeout++ >= UPDATE_TIMEOUT)
    {
        usInputRegisters[8] = 1;
        update_signal_timeout = 0;
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
        STOP_GENERATION;

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
        {
            STOP_GENERATION;

            ADC2->SR = ~ADC2->SR;
            cur_fault_delay = COIL_CURRENT_FAULT_DELAY;
            led_pin.reset();
        }
        __enable_irq();
    }
    ADC1->SR = ~ADC1->SR;
}

void system_monitor()
{
    if (ADC_END_INJ_CONVERSION(ADC1))
    {
        ADC_CLEAR_STATUS(ADC1);
        usInputRegisters[INPUT_REG_REF_VOLTAGE] =
            smooth_value(ALPHA_SMOOTH_VALUE, get_adc_ref_voltage(ADC1->JDR1), usInputRegisters[INPUT_REG_REF_VOLTAGE]);
        usInputRegisters[INPUT_REG_BAT_VOLTAGE] =
            smooth_value(ALPHA_SMOOTH_VALUE, get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC1->JDR2), 10000, 5100), usInputRegisters[INPUT_REG_BAT_VOLTAGE]);
        usInputRegisters[INPUT_REG_DC_VOLTAGE] =
            smooth_value(ALPHA_SMOOTH_VALUE, get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC1->JDR3), 1000, 100), usInputRegisters[INPUT_REG_DC_VOLTAGE]);

        // if (system_monitor_handler(usInputRegisters[INPUT_REG_REF_VOLTAGE], usInputRegisters[INPUT_REG_BAT_VOLTAGE], usInputRegisters[INPUT_REG_DC_VOLTAGE]) != SYSTEM_OK)
            // NVIC_SystemReset();
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
