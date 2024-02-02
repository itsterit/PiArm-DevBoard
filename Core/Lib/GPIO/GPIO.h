#ifndef _GPIO_H_
#define _GPIO_H_
#include <stm32f103xb.h>

class GPIO
{
private:
    GPIO_TypeDef *GPIOx;
    uint8_t PinNumber;

public:
    GPIO(GPIO_TypeDef *GPIOx, uint8_t PinNumber);
    bool clock_enable(bool clock_status);
};

/* _GPIO_H_ */
#endif