#ifndef _CLOCK_CONTROL_H_
#define _CLOCK_CONTROL_H_
#include <stm32f103xb.h>

class clock_control
{
public:
    typedef enum
    {
        HSI_SELECTED_AS_SYSTEM_CLOCK = 0b00,
        HSE_SELECTED_AS_SYSTEM_CLOCK = 0b01,
        PLL_SELECTED_AS_SYSTEM_CLOCK = 0b10,
    } SYSTEM_CLOCK_SOURCE_Type;
    static bool clock_switch(SYSTEM_CLOCK_SOURCE_Type system_clock_source);

    static bool set_ahb_prescaler(uint16_t prescaler_value);

    class hse
    {
    public:
        static bool enable(bool status);
        static bool ready(void);
        static bool clock_bypass_mode(bool status);
        static bool enable_security_system(bool status);
    };

    class hsi
    {
    public:
        static bool enable(bool status);
        static bool ready(void);
    };

    typedef enum
    {
        HSI_oscillator_divided_by_2 = 0b00,
        HSE_oscillator = 0b01,
    } PLL_CLOCK_SOURCE_Type;
    typedef enum
    {
        PLL_INPUT_CLOCK_X2 = 0b0000,
        PLL_INPUT_CLOCK_X3 = 0b0001,
        PLL_INPUT_CLOCK_X4 = 0b0010,
        PLL_INPUT_CLOCK_X5 = 0b0011,
        PLL_INPUT_CLOCK_X6 = 0b0100,
        PLL_INPUT_CLOCK_X7 = 0b0101,
        PLL_INPUT_CLOCK_X8 = 0b0110,
        PLL_INPUT_CLOCK_X9 = 0b0111,
        PLL_INPUT_CLOCK_X10 = 0b1000,
        PLL_INPUT_CLOCK_X11 = 0b1001,
        PLL_INPUT_CLOCK_X12 = 0b1010,
        PLL_INPUT_CLOCK_X13 = 0b1011,
        PLL_INPUT_CLOCK_X14 = 0b1100,
        PLL_INPUT_CLOCK_X15 = 0b1101,
        PLL_INPUT_CLOCK_X16 = 0b1110,
    } MULTIPLICATION_FACTOR_Type;
    class pll
    {
    public:
        static bool hse_clock_divided(bool status);
        static bool pll_clock_source(PLL_CLOCK_SOURCE_Type clock_source);
        static bool enable(bool status);
        static bool ready(void);
        static bool multiplication_factor(MULTIPLICATION_FACTOR_Type pll_multiplication);
    };
};

#endif /* _CLOCK_CONTROL_H_ */
