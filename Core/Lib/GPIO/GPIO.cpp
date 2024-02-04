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
    volatile uint32_t *configuration_register  = ((pin_number > 7) ? &(GPIOx->CRH) : &(GPIOx->CRL));

    *configuration_register &= ~GPIO_CRH_CNF11_1;
    *configuration_register &= ~GPIO_CRH_CNF11_0;
    *configuration_register |= (0b11 << GPIO_CRH_MODE11_Pos);

    return 0;
}