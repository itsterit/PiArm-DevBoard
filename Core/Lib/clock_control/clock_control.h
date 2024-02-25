#ifndef _CLOCK_CONTROL_H_
#define _CLOCK_CONTROL_H_
#include <stm32f103xb.h>

class clock_control
{
public:
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

    typedef enum
    {
        HSI_oscillator_divided_by_2 = 0b00,
        HSE_oscillator = 0b01,
    } PLL_CLOCK_SOURCE_Type;

    typedef enum
    {
        HSI_SELECTED_AS_SYSTEM_CLOCK = 0b00,
        HSE_SELECTED_AS_SYSTEM_CLOCK = 0b01,
        PLL_SELECTED_AS_SYSTEM_CLOCK = 0b10,
    } SYSTEM_CLOCK_SOURCE_Type;

    typedef enum
    {
        SYSCLK_NOT_DIVIDED = 0b0000,
        SYSCLK_DIVIDED_BY_2 = 0b1000,
        SYSCLK_DIVIDED_BY_4 = 0b1001,
        SYSCLK_DIVIDED_BY_8 = 0b1010,
        SYSCLK_DIVIDED_BY_16 = 0b1011,
        SYSCLK_DIVIDED_BY_64 = 0b1100,
        SYSCLK_DIVIDED_BY_128 = 0b1101,
        SYSCLK_DIVIDED_BY_256 = 0b1110,
        SYSCLK_DIVIDED_BY_512 = 0b1111,
    } AHB_PRESCALER_Type;

    typedef enum
    {
        HCLK_NOT_DIVIDED = 0b000,
        HCLK_DIVIDED_BY_2 = 0b100,
        HCLK_DIVIDED_BY_4 = 0b101,
        HCLK_DIVIDED_BY_8 = 0b110,
        HCLK_DIVIDED_BY_16 = 0b111,
    } APB1_PRESCALER_Type,
        APB2_PRESCALER_Type;

    typedef enum
    {
        PCLK2_DIVIDED_BY_2 = 0b00,
        PCLK2_DIVIDED_BY_4 = 0b01,
        PCLK2_DIVIDED_BY_6 = 0b10,
        PCLK2_DIVIDED_BY_8 = 0b11,
    } ADC_PRESCALER_Type;

    static bool clock_switch(SYSTEM_CLOCK_SOURCE_Type system_clock_source);
    static bool set_ahb_prescaler(AHB_PRESCALER_Type prescaler_value);
    static bool set_apb1_prescaler(APB1_PRESCALER_Type prescaler_value);
    static bool set_apb2_prescaler(APB2_PRESCALER_Type prescaler_value);
    static bool set_adc_prescaler(ADC_PRESCALER_Type prescaler_value);

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
