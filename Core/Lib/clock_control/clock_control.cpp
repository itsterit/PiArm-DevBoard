#include "clock_control.h"

bool clock_control::clock_switch(SYSTEM_CLOCK_SOURCE_Type system_clock_source)
{
    uint16_t start_counter = 0;
    RCC->CFGR &= ~RCC_CFGR_SW_Msk;
    RCC->CFGR |= (system_clock_source << RCC_CFGR_SW_Pos);

    do
    {
        if (((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos) == system_clock_source)
        {
            return 1;
        }
    } while (start_counter++ < 0xFFF);
    return 0;
}

bool clock_control::hse::enable(bool status)
{
    if (status)
        RCC->CR |= RCC_CR_HSEON_Msk;
    else
        RCC->CR &= ~RCC_CR_HSEON_Msk;

    asm("NOP");
    return (((RCC->CR & RCC_CR_HSEON_Msk) >> RCC_CR_HSEON_Pos) == status);
}

bool clock_control::hse::ready()
{
    uint16_t start_counter = 0;
    do
    {
        if (RCC->CR & RCC_CR_HSERDY_Msk)
        {
            return 1;
        }
    } while (start_counter++ < 0xFFF);
    return 0;
}

bool clock_control::hse::clock_bypass_mode(bool status)
{
    if (status)
        RCC->CR |= RCC_CR_HSEBYP_Msk;
    else
        RCC->CR &= ~RCC_CR_HSEBYP_Msk;

    asm("NOP");
    return (((RCC->CR & RCC_CR_HSEBYP_Msk) >> RCC_CR_HSEBYP_Pos) == status);
}

/**
 * @brief   Система мониторинга внешнего кварца
 * @warning Обнаружив сбой генерит прерывание, переключая систему тактирования на HSI
 */
bool clock_control::hse::enable_security_system(bool status)
{
    if (status)
        RCC->CR |= RCC_CR_CSSON_Msk;
    else
        RCC->CR &= ~RCC_CR_CSSON_Msk;

    asm("NOP");
    return (((RCC->CR & RCC_CR_CSSON_Msk) >> RCC_CR_CSSON_Pos) == status);
}

bool clock_control::hsi::enable(bool status)
{
    if (status)
        RCC->CR |= RCC_CR_HSION_Msk;
    else
        RCC->CR &= ~RCC_CR_HSION_Msk;

    asm("NOP");
    return (((RCC->CR & RCC_CR_HSION_Msk) >> RCC_CR_HSION_Pos) == status);
}

bool clock_control::hsi::ready()
{
    uint16_t start_counter = 0;
    do
    {
        if (RCC->CR & RCC_CR_HSIRDY_Msk)
        {
            return 1;
        }
    } while (start_counter++ < 0xFFF);
    return 0;
}

/**
 * @brief HSE clock divided by 2
 */
bool clock_control::pll::hse_clock_divided(bool status)
{
    RCC->CR &= ~RCC_CR_PLLON_Msk;

    if (status)
        RCC->CFGR |= RCC_CFGR_PLLXTPRE_Msk;
    else
        RCC->CFGR &= ~RCC_CFGR_PLLXTPRE_Msk;

    asm("NOP");
    return (((RCC->CFGR & RCC_CFGR_PLLXTPRE_Msk) >> RCC_CFGR_PLLXTPRE_Pos) == status);
}

bool clock_control::pll::pll_clock_source(PLL_CLOCK_SOURCE_Type clock_source)
{
    RCC->CR &= ~RCC_CR_PLLON_Msk;

    RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
    RCC->CFGR |= (clock_source << RCC_CFGR_PLLSRC_Pos);

    asm("NOP");
    return (((RCC->CFGR & RCC_CFGR_PLLSRC_Msk) >> RCC_CFGR_PLLSRC_Pos) == clock_source);
}

bool clock_control::pll::enable(bool status)
{
    if (status)
        RCC->CR |= RCC_CR_PLLON_Msk;
    else
        RCC->CR &= ~RCC_CR_PLLON_Msk;

    asm("NOP");
    return (((RCC->CR & RCC_CR_PLLON_Msk) >> RCC_CR_PLLON_Pos) == status);
}

bool clock_control::pll::ready()
{
    uint16_t start_counter = 0;
    do
    {
        if (RCC->CR & RCC_CR_PLLRDY_Msk)
        {
            return 1;
        }
    } while (start_counter++ < 0xFFF);
    return 0;
}

bool clock_control::pll::multiplication_factor(MULTIPLICATION_FACTOR_Type pll_multiplication)
{
    RCC->CR &= ~RCC_CR_PLLON_Msk;

    RCC->CFGR &= ~RCC_CFGR_PLLMULL_Msk;
    RCC->CFGR |= (pll_multiplication << RCC_CFGR_PLLMULL_Pos);

    asm("NOP");
    return (((RCC->CFGR & RCC_CFGR_PLLMULL_Msk) >> RCC_CFGR_PLLMULL_Pos) == pll_multiplication);
}

bool clock_control::set_ahb_prescaler(AHB_PRESCALER_Type prescaler_value)
{
    RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
    RCC->CFGR |= (prescaler_value << RCC_CFGR_HPRE_Pos);

    FLASH->ACR &= ~FLASH_ACR_PRFTBE_Msk;
    if (prescaler_value > 0)
        FLASH->ACR |= FLASH_ACR_PRFTBE_Msk;

    asm("NOP");
    return ((RCC->CFGR & RCC_CFGR_HPRE_Msk) >> RCC_CFGR_HPRE_Pos == prescaler_value);
}

bool clock_control::set_apb1_prescaler(APB1_PRESCALER_Type prescaler_value)
{
    RCC->CFGR &= ~RCC_CFGR_PPRE1_Msk;
    RCC->CFGR |= (prescaler_value << RCC_CFGR_PPRE1_Pos);

    asm("NOP");
    return (((RCC->CFGR & RCC_CFGR_PPRE1_Msk) >> RCC_CFGR_PPRE1_Pos) == prescaler_value);
}

bool clock_control::set_apb2_prescaler(APB2_PRESCALER_Type prescaler_value)
{
    RCC->CFGR &= ~RCC_CFGR_PPRE2_Msk;
    RCC->CFGR |= (prescaler_value << RCC_CFGR_PPRE2_Pos);

    asm("NOP");
    return (((RCC->CFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos) == prescaler_value);
}

bool clock_control::set_adc_prescaler(ADC_PRESCALER_Type prescaler_value)
{
    RCC->CFGR &= ~RCC_CFGR_ADCPRE_Msk;
    RCC->CFGR |= (prescaler_value << RCC_CFGR_ADCPRE_Pos);

    asm("NOP");
    return (((RCC->CFGR & RCC_CFGR_ADCPRE_Msk) >> RCC_CFGR_ADCPRE_Pos) == prescaler_value);
}