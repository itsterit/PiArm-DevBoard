#include "timer.h"

timer::timer(TIM_TypeDef *timer)
{
    act_timer = timer;
    act_timer->SR = ~act_timer->SR;

    if (act_timer == TIM1)
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    else if (act_timer == TIM2)
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    else if (act_timer == TIM3)
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    else if (act_timer == TIM4)
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
}

void timer::set_channel_output_config(uint8_t channel,
                                      OUTPUT_COMPARE_CLEAR_Type output_compare_clear,
                                      OUTPUT_COMPARE_PRELOAD_Type output_compare_preload,
                                      OUTPUT_COMPARE_FAST_Type output_compare_fast,
                                      OUTPUT_COMPARE_MODE_Type output_mode)
{
    if ((channel) && (channel <= 4) && (act_timer))
    {
        volatile uint32_t *configuration_register = ((channel > 2) ? &(act_timer->CCMR2) : &(act_timer->CCMR1));
        volatile uint8_t channel_configuration_shift = (((channel == 1) || (channel == 3)) ? (0) : (8));

        *configuration_register &= ~((TIM_CCMR1_OC1CE_Msk >> TIM_CCMR1_OC1CE_Pos) << (TIM_CCMR1_OC1CE_Pos + channel_configuration_shift));
        *configuration_register |= (output_compare_clear << (TIM_CCMR1_OC1CE_Pos + channel_configuration_shift));

        *configuration_register &= ~((TIM_CCMR1_OC1M_Msk >> TIM_CCMR1_OC1M_Pos) << (TIM_CCMR1_OC1M_Pos + channel_configuration_shift));
        *configuration_register |= (output_mode << (TIM_CCMR1_OC1M_Pos + channel_configuration_shift));

        *configuration_register &= ~((TIM_CCMR1_OC1PE_Msk >> TIM_CCMR1_OC1PE_Pos) << (TIM_CCMR1_OC1PE_Pos + channel_configuration_shift));
        *configuration_register |= (output_compare_preload << (TIM_CCMR1_OC1PE_Pos + channel_configuration_shift));

        *configuration_register &= ~((TIM_CCMR1_OC1FE_Msk >> TIM_CCMR1_OC1FE_Pos) << (TIM_CCMR1_OC1FE_Pos + channel_configuration_shift));
        *configuration_register |= (output_compare_fast << (TIM_CCMR1_OC1FE_Pos + channel_configuration_shift));

        *configuration_register &= ~((TIM_CCMR1_CC1S_Msk >> TIM_CCMR1_CC1S_Pos) << (TIM_CCMR1_CC1S_Pos + channel_configuration_shift));
    }
}

void timer::set_event_generation(TRIGGER_GENERATION_Type trigger_generation,
                                 UPDATE_GENERATION_Type update_generation,
                                 uint32_t capture_compare_generation_msk)
{
    if (act_timer)
    {
        act_timer->EGR = ((trigger_generation << TIM_EGR_TG_Pos) | (update_generation << TIM_EGR_UG_Pos) |
                          ((TIM_EGR_CC4G_Msk | TIM_EGR_CC3G_Msk | TIM_EGR_CC2G_Msk | TIM_EGR_CC1G_Msk) & capture_compare_generation_msk));
    }
}

void timer::set_dma_interrupt_config(TRIGGER_DMA_REQUEST_Type trigger_dma_request,
                                     UPDATE_DMA_REQUEST_Type update_dma_request,
                                     TRIGGER_INTERRUPT_Type trigger_interrupt,
                                     UPDATE_INTERRUPT_Type update_interrupt,
                                     uint32_t capture_compare_dma_request_enable_msk,
                                     uint32_t capture_compare_interrupt_enable_msk)
{
    if (act_timer)
    {
        act_timer->DIER = ((trigger_dma_request << TIM_DIER_TDE_Pos) | (update_dma_request << TIM_DIER_UDE_Pos) | (trigger_interrupt << TIM_DIER_TIE_Pos) | (update_interrupt << TIM_DIER_UIE_Pos) |
                           ((TIM_DIER_CC4DE_Msk | TIM_DIER_CC3DE_Msk | TIM_DIER_CC2DE_Msk | TIM_DIER_CC1DE_Msk) & capture_compare_dma_request_enable_msk) |
                           ((TIM_DIER_CC4IE_Msk | TIM_DIER_CC3IE_Msk | TIM_DIER_CC2IE_Msk | TIM_DIER_CC1IE_Msk) & capture_compare_interrupt_enable_msk));
    }
}

void timer::slave_mode_control(TRIGGER_SELECTION_Type trigger_selection, SLAVE_MODE_SELECTION_Type slave_mode_selection)
{
    if (act_timer)
    {
        act_timer->SMCR = (trigger_selection << TIM_SMCR_TS_Pos) | (slave_mode_selection << TIM_SMCR_SMS_Pos);
    }
}

void timer::master_mode_config(MASTER_MODE_SELECTION_Type master_mode)
{
    if (act_timer)
    {
        act_timer->CR2 = (master_mode << TIM_CR2_MMS_Pos);
    }
}

void timer::capture_compare_register(uint32_t channel_output_polarity_inverted_msk, uint32_t channel_output_enable_msk)
{
    if (act_timer)
    {
        act_timer->CCER = (((TIM_CCER_CC4P_Msk | TIM_CCER_CC3P_Msk | TIM_CCER_CC2P_Msk | TIM_CCER_CC1P_Msk) & channel_output_polarity_inverted_msk) |
                           ((TIM_CCER_CC4E_Msk | TIM_CCER_CC3E_Msk | TIM_CCER_CC2E_Msk | TIM_CCER_CC1E_Msk) & channel_output_enable_msk));
    }
}

void timer::set_counter_config(AUTO_RELOAD_PRELOAD_Type auto_reload_preload, COUNTER_DIRECTION_Type counter_direction, ONE_PULSE_MODE_Type one_pulse_mode, COUNTER_ENABLE_Type counter_enable)
{
    if (act_timer)
    {
        act_timer->CR1 = (auto_reload_preload << TIM_CR1_ARPE_Pos) | (counter_direction << TIM_CR1_DIR_Pos) | (one_pulse_mode << TIM_CR1_OPM_Pos) | (counter_enable << TIM_CR1_CEN_Pos);
    }
}

void timer::set_timer_config(uint16_t capture_compare_register1_val,
                             uint16_t capture_compare_register2_val,
                             uint16_t capture_compare_register3_val,
                             uint16_t capture_compare_register4_val,
                             uint16_t auto_reload_register, uint16_t prescaler_val, uint16_t counter_val)
{
    if (act_timer)
    {
        act_timer->CCR1 = capture_compare_register1_val;
        act_timer->CCR2 = capture_compare_register2_val;
        act_timer->CCR3 = capture_compare_register3_val;
        act_timer->CCR4 = capture_compare_register4_val;
        act_timer->ARR = auto_reload_register;
        act_timer->PSC = prescaler_val;
        act_timer->CNT = counter_val;
    }
}