#ifndef _GPIO_H_
#define _GPIO_H_
#include <stm32f103xb.h>

class GPIO
{
private:
    GPIO_TypeDef *GPIOx;
    uint8_t pin_number;
    bool pin_data_read_access{0};
    volatile uint32_t *pin_level;

public:
    typedef enum
    {
        output_push_pull = 0b00,
        output_open_drain = 0b01,
    } PIN_OUTPUT_Type;

    typedef enum
    {
        alternate_push_pull = 0b10,
        alternate_open_drain = 0b11,
    } PIN_ALTERNATE_Type;

    typedef enum
    {
        input_analog = 0b00000,
        input_floating = 0b01000,
        input_pull_down = 0b10000,
        input_pull_up = 0b10001,
    } PIN_INPUT_Type;

    GPIO(GPIO_TypeDef *GPIOx, uint8_t pin_number);
    bool clock_enable(bool clock_status);
    bool set_config(PIN_INPUT_Type pin_input_config);
    bool set_config(PIN_ALTERNATE_Type pin_alternate_config);
    bool set_config(PIN_OUTPUT_Type pin_output_config);
    void set(void)
    {
        GPIOx->BSRR = (0b01 << pin_number);
    }
    void reset(void)
    {
        GPIOx->BRR = (0b01 << pin_number);
    }
    bool get_level(void)
    {
        return (*pin_level & (pin_data_read_access << pin_number));
    }
};

/* _GPIO_H_ */
#endif