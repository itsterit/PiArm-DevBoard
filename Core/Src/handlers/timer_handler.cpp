#include "main.h"

void config_timer(uint32_t tmr_freq, uint16_t frq, uint8_t duty)
{
    uint32_t timer_arr = tmr_freq / frq;
    uint32_t timer_main_channel = (timer_arr / 100) * duty;
    uint8_t start_sampling_offset = 10;
    uint32_t start_coil_reply_sampling = (timer_main_channel + start_sampling_offset);
    uint32_t end_coil_reply_sampling = (timer_arr - start_sampling_offset);
    uint32_t start_coil_toque_sampling = start_sampling_offset;

    coil_frequency_timer.set_timer_config(start_coil_reply_sampling, timer_main_channel, start_coil_toque_sampling, end_coil_reply_sampling, timer_arr, 71, 0);
}

extern "C" void TIM1_UP_IRQHandler(void)
{
    TIM1->SR = ~TIM1->SR;

    GPIOB->BSRR = (0b01 << 11U);
    GPIOB->BSRR = (0b01 << 11U);
    GPIOB->BSRR = (0b01 << 11U);
    GPIOB->BSRR = (0b01 << 11U);
    GPIOB->BRR = (0b01 << 11U);
}

extern "C" void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_CC3IF_Msk)
    {
        TIM1->CR1 |= (0b01 << TIM_CR1_CEN_Pos);
    }
    else
    {
        TIM1->CR1 &= ~(0b01 << TIM_CR1_CEN_Pos);
    }

    TIM3->SR = ~TIM3->SR;
}
