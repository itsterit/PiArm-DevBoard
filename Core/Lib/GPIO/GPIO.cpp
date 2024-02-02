#include "GPIO.h"

/**
 * @brief               Создание экземпляра класса
 * @param[in] GPIOx     x=(A..G)
 * @param[in] PinNumber номер контакта
 */
GPIO::GPIO(GPIO_TypeDef *GPIOx, uint8_t PinNumber)
{
    this->GPIOx = GPIOx;
    this->PinNumber = PinNumber;
}

/**
 * @brief   Запуск тактирования
 * @note    Знаю, что рассчитать смещение можно по адресу GPIO,
 *          но что-то мне похуй.
 */
bool GPIO::clock_enable(bool clock_status)
{
    uint32_t apb_bus_shift = 0;

    if (GPIOx == GPIOA)
    {
        apb_bus_shift = RCC_APB2ENR_IOPAEN_Msk;
    }
    else if (GPIOx == GPIOB)
    {
        apb_bus_shift = RCC_APB2ENR_IOPBEN_Msk;
    }
    else if (GPIOx == GPIOC)
    {
        apb_bus_shift = RCC_APB2ENR_IOPCEN_Msk;
    }
    else if (GPIOx == GPIOD)
    {
        apb_bus_shift = RCC_APB2ENR_IOPDEN_Msk;
    }
    else if (GPIOx == GPIOE)
    {
        apb_bus_shift = RCC_APB2ENR_IOPEEN_Msk;
    }
    else
    {
        return 0;
    }

    if (clock_status)
    {
        RCC->APB2ENR |= apb_bus_shift;
    }
    else
    {
        RCC->APB2ENR &= ~apb_bus_shift;
    }

    return 1;
}