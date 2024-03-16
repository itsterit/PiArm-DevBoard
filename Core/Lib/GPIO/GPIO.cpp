#include "GPIO.h"

/**
 * @brief               Создание экземпляра класса
 * @param[in] GPIOx     x=(A..G)
 * @param[in] pin_number номер контакта
 */
GPIO::GPIO(GPIO_TypeDef *GPIOx, uint8_t pin_number)
{
    this->GPIOx = GPIOx;
    this->pin_number = (pin_number & 0b1111);
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

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    if (clock_status)
        RCC->APB2ENR |= (apb_bus_mask);
    else
        RCC->APB2ENR &= ~(apb_bus_mask);

    asm("NOP");
    return (((RCC->APB2ENR & apb_bus_mask) >> apb_bus_shift) == clock_status);
}

/**
 * @brief       Настройка пина на вход
 *
 * @note        На настройку состояния выходного регистра кладется болтяра, кроме режимов с подтяжкой(см.доку)
 * @param[in]   pin_input_config PIN_INPUT_Type
 */
bool GPIO::set_config(PIN_INPUT_Type pin_input_config)
{
    pin_data_read_access = 1;
    pin_level = &(GPIOx->IDR);

    volatile uint32_t *configuration_register = ((pin_number > 7) ? &(GPIOx->CRH) : &(GPIOx->CRL));
    volatile uint8_t mode_shift = ((pin_number > 7) ? ((pin_number - 8) * 4) : ((pin_number) * 4));
    volatile uint8_t cnfg_shift = mode_shift + 2;

    *configuration_register &= ~(0b11 << cnfg_shift);
    *configuration_register |= ((pin_input_config >> 3) << cnfg_shift);
    *configuration_register &= ~(0b11 << mode_shift);
    GPIOx->ODR &= ~(0b01 << pin_number);
    GPIOx->ODR |= ((pin_input_config & 0b01) << pin_number);

    asm("NOP");
    return ((((*configuration_register & (0b11 << cnfg_shift)) >> cnfg_shift) == (pin_input_config >> 3)) &&
            (((*configuration_register & (0b11 << mode_shift)) >> mode_shift) == 0b00) &&
            (((GPIOx->ODR & (0b1 << pin_number)) >> pin_number) == (pin_input_config & 0b01)));
}

/**
 * @brief       Настройка пина на альтернативный функционал
 *
 * @note        На настройку состояния выходного регистра кладется болтяра
 * @details     Устанавливает push_pull или open_drain
 * @param[in]   pin_alternate_config PIN_ALTERNATE_Type
 */
bool GPIO::set_config(PIN_ALTERNATE_Type pin_alternate_config, ALTERNATE_MODE_Type alternate_mode_config)
{
    pin_data_read_access = 0;
    volatile uint32_t *configuration_register = ((pin_number > 7) ? &(GPIOx->CRH) : &(GPIOx->CRL));
    volatile uint8_t mode_shift = ((pin_number > 7) ? ((pin_number - 8) * 4) : ((pin_number) * 4));
    volatile uint8_t cnfg_shift = mode_shift + 2;

    *configuration_register &= ~(0b11 << cnfg_shift);
    *configuration_register &= ~(0b11 << mode_shift);
    *configuration_register |= (pin_alternate_config << cnfg_shift);
    *configuration_register |= ((alternate_mode_config >> 1) << mode_shift);

    GPIOx->ODR &= ~(0b01 << pin_number);
    GPIOx->ODR |= ((alternate_mode_config & 0b01) << pin_number);

    asm("NOP");
    return ((((*configuration_register & (0b11 << cnfg_shift)) >> cnfg_shift) == pin_alternate_config) &&
            (((*configuration_register & ((alternate_mode_config >> 1) << mode_shift)) >> mode_shift) == 0b11));
}

/**
 * @brief     Настройка пина как выход
 *
 * @note      Настройка состояния выходного регистра определяет состояние ножки
 * @details   Устанавливает push_pull или open_drain
 * @param[in] pin_output_config PIN_OUTPUT_Type
 */
bool GPIO::set_config(PIN_OUTPUT_Type pin_output_config)
{
    pin_data_read_access = 1;
    pin_level = &(GPIOx->ODR);

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
