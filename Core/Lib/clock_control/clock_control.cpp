#include "clock_control.h"

bool clock_control::clock_switch(SYSTEM_CLOCK_SOURCE_Type system_clock_source)
{
    uint16_t start_counter = 0;
    RCC->CFGR &= ~RCC_CFGR_SW_Msk;
    RCC->CFGR |= (system_clock_source << RCC_CFGR_SW_Pos);

    // do
    // {
    //     if (((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos) == system_clock_source)
    //     {
            return 1;
    //     }
    // } while (start_counter++ < 0xFFF);
    // return 0;
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

bool clock_control::set_ahb_prescaler(uint16_t prescaler_value)
{
    switch (prescaler_value)
    {
    case 2:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV2;
        break;
    case 4:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV4;
        break;
    case 8:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV8;
        break;
    case 16:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV16;
        break;
    case 64:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV64;
        break;
    case 128:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV128;
        break;
    case 256:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV256;
        break;
    case 512:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV512;
        break;
    default:
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
        return 0;
    }
    return 1;
}

bool clock_control::set_apb1_prescaler(uint16_t prescaler_value)
{
    switch (prescaler_value)
    {
    case 1:
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
        break;
    case 2:
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
        break;
    case 4:
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
        break;
    case 8:
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV8;
        break;
    case 16:
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV16;
        break;
    default:
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
        return 0;
    }
    return 1;
}

bool clock_control::set_apb2_prescaler(uint16_t prescaler_value)
{
    switch (prescaler_value)
    {
    case 1:
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
        break;
    case 2:
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
        break;
    case 4:
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV4;
        break;
    case 8:
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV8;
        break;
    case 16:
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV16;
        break;
    default:
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
        return 0;
    }
    return 1;
}

bool clock_control::set_adc_prescaler(uint16_t prescaler_value)
{
    switch (prescaler_value)
    {
    case 2:
        RCC->CFGR |= RCC_CFGR_ADCPRE_DIV2;
        break;
    case 4:
        RCC->CFGR |= RCC_CFGR_ADCPRE_DIV4;
        break;
    case 6:
        RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;
        break;
    case 8:
        RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;
        break;
    default:
        RCC->CFGR |= RCC_CFGR_ADCPRE_DIV2;
        return 0;
    }
    return 1;
}