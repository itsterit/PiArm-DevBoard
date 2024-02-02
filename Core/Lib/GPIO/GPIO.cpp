#include "GPIO.h"

/**
 * @brief               Создание экземпляра класса
 * @param[in] GPIOx     x=(A..G)
 * @param[in] PinNumber номер контакта
 */
GPIO::GPIO(GPIO_TypeDef *GPIOx, uint8_t PinNumber)
{
    this->GPIOx     = GPIOx;
    this->PinNumber = PinNumber;
}

/**
 * @brief   Запуск тактирования
 * @note    Знаю, что рассчитать смещение можно по адресу GPIO,
 *          но что-то мне (не)похуй.
 */
bool GPIO::clock_enable(bool clock_status)
{
    uint32_t gpio_position = ((uint32_t)GPIOx - (uint32_t)GPIOA) / ((uint32_t)GPIOB - (uint32_t)GPIOA);
    uint32_t apb_bus_shift = (gpio_position + RCC_APB2ENR_IOPAEN_Pos);
    uint32_t apb_bus_mask  = (0b01 << apb_bus_shift);

    if (clock_status)
        RCC->APB2ENR |= (apb_bus_mask);
    else
        RCC->APB2ENR &= ~(apb_bus_mask);

    asm("NOP");
    return (((RCC->APB2ENR & apb_bus_mask) >> apb_bus_shift) == clock_status);
}