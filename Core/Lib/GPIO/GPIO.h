#ifndef _GPIO_H_
#define _GPIO_H_
#include <stm32f103xb.h>

class GPIO
{
private:
    GPIO_TypeDef *GPIOx;
    uint8_t pin_number;

public:
    typedef enum
    {
        output_push_pull  = 0b00,
        output_open_drain = 0b01,
    } PIN_OUTPUT_Type;

    typedef enum
    {
        alternate_push_pull,
        alternate_open_drain,
    } PIN_ALTERNATE_Type;

    typedef enum
    {
        input_analog = 0,
        input_floating,
        input_pull_down,
        input_pull_up,
    } PIN_INPUT_Type;

    GPIO(GPIO_TypeDef *GPIOx, uint8_t pin_number);
    bool clock_enable(bool clock_status);
    bool set_config(PIN_INPUT_Type pin_input_config);
    bool set_config(PIN_ALTERNATE_Type pin_alternate_config);
    bool set_config(PIN_OUTPUT_Type pin_output_config);
};

/* _GPIO_H_ */
#endif