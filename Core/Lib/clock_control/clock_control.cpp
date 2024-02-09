#include "clock_control.h"

bool clock_control::clock_switch(SYSTEM_CLOCK_SOURCE_Type system_clock_source)
{
    uint32_t start_counter = 0;
    RCC->CFGR &= ~RCC_CFGR_SW_Msk;
    RCC->CFGR |= (system_clock_source << RCC_CFGR_SW_Pos);

    do
    {
        if (((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos) == system_clock_source)
        {
            return 1;
        }
    } while (start_counter < 0xFFFFFFFF);
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

bool clock_control::hse::clock_ready()
{
    uint32_t start_counter = 0;
    do
    {
        if (RCC->CR & RCC_CR_HSERDY_Msk)
        {
            return 1;
        }
    } while (start_counter < 0xFFFFFFFF);
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