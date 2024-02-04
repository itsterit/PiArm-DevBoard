#include "GPIO.h"

/**
 * @brief               Создание экземпляра класса
 * @param[in] GPIOx     x=(A..G)
 * @param[in] pin_number номер контакта
 */
GPIO::GPIO(GPIO_TypeDef *GPIOx, uint8_t pin_number)
{
    this->GPIOx = GPIOx;
    this->pin_number = pin_number;
}

/**
 * @brief   Запуск тактирования
 * @note    Знаю, что рассчитать смещение можно по адресу GPIO,
 *          но что-то мне (не)похуй.
 */
bool GPIO::clock_enable(bool clock_status)
{
    volatile uint32_t gpio_position = ((uint32_t)GPIOx - (uint32_t)GPIOA) / ((uint32_t)GPIOB - (uint32_t)GPIOA);
    volatile uint32_t apb_bus_shift = (gpio_position + RCC_APB2ENR_IOPAEN_Pos);
    volatile uint32_t apb_bus_mask = (0b01 << apb_bus_shift);

    if (clock_status)
        RCC->APB2ENR |= (apb_bus_mask);
    else
        RCC->APB2ENR &= ~(apb_bus_mask);

    asm("NOP");
    return (((RCC->APB2ENR & apb_bus_mask) >> apb_bus_shift) == clock_status);
}

/**
 * @brief     -
 * @details   -
 * @param[in] -
 */
bool GPIO::set_config(PIN_INPUT_Type pin_input_config)
{
    return 0;
}

/**
 * @brief     -
 * @details   -
 * @param[in] -
 */
bool GPIO::set_config(PIN_ALTERNATE_Type pin_alternate_config)
{
    return 0;
}

/**
 * @brief     -
 * @details   -
 * @param[in] -
 */
bool GPIO::set_config(PIN_OUTPUT_Type pin_output_config)
{
    if (pin_number <= 15)
    {
        volatile uint32_t *configuration_register = ((pin_number > 7) ? &(GPIOx->CRH) : &(GPIOx->CRL));
        volatile uint8_t mode_shift = ((pin_number > 7) ? ((pin_number - 8) * 4) : ((pin_number) * 4));
        volatile uint8_t cnfg_shift = mode_shift + 2;

        *configuration_register &= ~(0b11 << cnfg_shift);
        *configuration_register |= (pin_output_config << cnfg_shift);
        *configuration_register |= (0b11 << mode_shift);

        asm("NOP");
        return ((((*configuration_register & (0b11 << cnfg_shift)) >> cnfg_shift) == pin_output_config) &&
                (((*configuration_register & (0b11 << mode_shift)) >> mode_shift) == 0b11));
    }
    return 0;
}