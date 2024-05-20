#include "main.h"

void set_timer_config()
{
    check_system_parameters();
    gen_freq.clock_enable(true);
    gen_freq.set_config(GPIO::alternate_push_pull, GPIO::alternate_output_mode);
    AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP_Msk;
    AFIO->MAPR |= (0b01 << AFIO_MAPR_TIM3_REMAP_PARTIALREMAP_Pos);

    // Таймер сэмплирования сигнала
    {
        sampling_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_DISABLE, 0, (TIM_DIER_CC1IE_Msk));
        sampling_timer.slave_mode_control(INTERNAL_TRIGGER2, TRIGGER_MODE);
        sampling_timer.set_timer_config(0, 0, 0, 0, 1, 56, 0);
        sampling_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_DISABLE, COUNTER_DISABLE);
        sampling_timer.master_mode_config(MASTER_MODE_COMPARE_PULSE);
        sampling_timer.set_channel_output_config(1, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_DISABLE, OUTPUT_COMPARE_FAST_DISABLE, CHANNEL_TOGGLE);
        sampling_timer.capture_compare_register(0, TIM_CCER_CC1E_Msk);
        sampling_timer.set_break_and_dead_time(OC_AND_OCN_OUTPUTS_ARE_ENABLED, MOE_CAN_BE_SET_ONLY_BY_SOFTWARE,
                                               BREAK_INPUT_BRK_IS_ACTIVE_HIGH, BREAK_INPUTS_DISABLED, WHEN_INACTIVE_OUTPUTS_DISABLED, WHEN_INACTIVE_DISABLED, LOCK_OFF, 0);
    }

    // задающий таймер
    {
#if INVERT_GENERATOR_SIGNAL
        coil_frequency_timer.set_channel_output_config(2U, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_ENABLE, OUTPUT_COMPARE_FAST_ENABLE, CHANNEL_PWM_MODE_2);
#else
        coil_frequency_timer.set_channel_output_config(2U, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_ENABLE, OUTPUT_COMPARE_FAST_ENABLE, CHANNEL_PWM_MODE_1);
#endif
        coil_frequency_timer.set_event_generation(TRIGGER_GENERATION_DISABLE, UPDATE_GENERATION_DISABLE, 0);
        coil_frequency_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_DISABLE, 0,
                                                      (TIM_DIER_CC3IE_Msk | TIM_DIER_CC4IE_Msk));
        coil_frequency_timer.slave_mode_control(INTERNAL_TRIGGER0, SLAVE_MODE_DISABLED);
        coil_frequency_timer.master_mode_config(MASTER_MODE_COMPARE_PULSE);
        coil_frequency_timer.capture_compare_register(0, TIM_CCER_CC2E_Msk);
        set_generation_timing(1000000, usHoldingRegisters[HOLDING_COIL_FREQUENCY], usHoldingRegisters[HOLDING_COIL_DUTY]);
        coil_frequency_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_DISABLE, COUNTER_ENABLE);
    }

    // Дма на сэмплирование
    {
        adc_samling_dma.dma_set_config(MEM2MEM_Disabled, PL_Low,
                                       MSIZE_16bits, PSIZE_16bits,
                                       MINC_Enabled, PINC_Disabled, CIRC_Disabled, Read_From_Peripheral,
                                       TEIE_Disabled, HTIE_Disabled, TCIE_Disabled);
        adc_samling_dma.dma_start(100, (uint32_t *)&usInputRegisters[10], (uint32_t *)&ADC1->DR);
    }
}

void set_generation_timing(uint32_t tmr_freq, uint16_t frq, uint8_t duty)
{
    uint8_t start_sampling_offset = 1;
    uint32_t timer_arr = tmr_freq / frq;
    uint32_t timer_main_channel = (timer_arr / 100) * duty;
    uint32_t start_coil_reply_sampling = (timer_main_channel + start_sampling_offset);
    uint32_t end_coil_reply_sampling = (timer_arr - 10);
    uint32_t start_coil_toque_sampling = timer_main_channel / 2;
    coil_frequency_timer.set_timer_config(start_coil_reply_sampling, timer_main_channel, start_coil_toque_sampling, end_coil_reply_sampling, timer_arr, 55, 0);
}

extern "C" void TIM1_CC_IRQHandler(void)
{
    TIM1->SR = ~TIM1->SR;
}

extern "C" void TIM3_IRQHandler(void)
{
    {
        if (TIM3->SR & TIM_SR_CC4IF_Msk)
        {
            /* Конец замера ответа катушки */
            TIM1->CR1 &= ~(TIM_CR1_CEN_Msk);
            TIM1->SR = ~TIM1->SR;
            TIM1->CNT = 0;
        }
        if (TIM3->SR & TIM_SR_CC3IF_Msk)
        {
            /* Начало замера тока катушки */
            act_coil_current = ADC2->DR;
        }
    }
    TIM3->SR = ~TIM3->SR;
}
