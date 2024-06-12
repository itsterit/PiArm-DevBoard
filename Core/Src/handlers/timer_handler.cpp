#include "main.h"
#define SAMPLING_POINT_AMOUNT 100

void set_timer_config()
{
    check_system_parameters();
    main_signal.signal_point_amt = 0;
    search_signal.signal_point_amt = 0;

    gen_freq.clock_enable(true);
    gen_freq.set_config(GPIO::alternate_push_pull, GPIO::alternate_output_mode);
    AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP_Msk;
    AFIO->MAPR |= (0b01 << AFIO_MAPR_TIM3_REMAP_PARTIALREMAP_Pos);

    // задающий таймер
    {
#if INVERT_GENERATOR_SIGNAL
        coil_frequency_timer.set_channel_output_config(2U, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_ENABLE, OUTPUT_COMPARE_FAST_ENABLE, CHANNEL_PWM_MODE_2, CC_CHANNEL_IS_AS_OUTPUT);
#else
        coil_frequency_timer.set_channel_output_config(2U, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_ENABLE, OUTPUT_COMPARE_FAST_ENABLE, CHANNEL_PWM_MODE_1, CC_CHANNEL_IS_AS_OUTPUT);
#endif
        coil_frequency_timer.set_event_generation(TRIGGER_GENERATION_DISABLE, UPDATE_GENERATION_DISABLE, 0);
        coil_frequency_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_DISABLE, 0,
                                                      (TIM_DIER_CC3IE | TIM_DIER_CC4IE));
        coil_frequency_timer.slave_mode_control(INTERNAL_TRIGGER0, SLAVE_MODE_DISABLED);
        coil_frequency_timer.master_mode_config(MASTER_MODE_COMPARE_PULSE);
        coil_frequency_timer.capture_compare_register(0, TIM_CCER_CC2E_Msk);
        set_generation_timing(1000000, usHoldingRegisters[HOLDING_COIL_FREQUENCY], usHoldingRegisters[HOLDING_COIL_DUTY]);
        coil_frequency_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_DISABLE, COUNTER_ENABLE);
    }

    // Звук
    {
        buzzer_timer.set_channel_output_config(4U, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_ENABLE, OUTPUT_COMPARE_FAST_ENABLE, CHANNEL_PWM_MODE_1, CC_CHANNEL_IS_AS_OUTPUT);
        buzzer_timer.set_event_generation(TRIGGER_GENERATION_DISABLE, UPDATE_GENERATION_DISABLE, 0);
        buzzer_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_DISABLE, 0, 0);
        buzzer_timer.capture_compare_register(0, TIM_CCER_CC4E_Msk);
        buzzer_timer.set_timer_config(0, 0, 0, 0, 500, ((main_frq / 1000000) - 1), 0);
        buzzer_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_DISABLE, COUNTER_ENABLE);
    }

    // таймер установки баланса грунта
    {
        set_balance_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_ENABLE, 0, 0);
        set_balance_timer.set_timer_config(0, 0, 0, 0, 200, ((main_frq / 1000) - 1), 0);
        set_balance_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_ENABLE, COUNTER_DISABLE);
    }

    // Таймер сэмплирования сигнала
    {
        sampling_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_DISABLE, 0, (0));
        sampling_timer.slave_mode_control(INTERNAL_TRIGGER2, TRIGGER_MODE);
        sampling_timer.set_timer_config(0, 0, 0, 0, 32000, 0, 0);
        sampling_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_ENABLE, COUNTER_DISABLE);

        TIM2->CCMR1 |= (0b01 << TIM_CCMR1_CC1S_Pos);         // выбираем TI2 для CH1
        TIM2->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC); // не фильтруем и делитель не используем
        TIM2->CCMR1 |= (0b0100 << TIM_CCMR1_IC1F_Pos);       //
        TIM2->CCER |= TIM_CCER_CC1P;                         // выбираем захват по заднему фронту
        TIM2->CCER |= TIM_CCER_CC1E;                         // включаем режим захвата для 1-го канала
        // TIM2->DIER |= TIM_DIER_CC1IE;                        // разрешаем прерывание по захвату

        sampling_timer.set_break_and_dead_time(OC_AND_OCN_OUTPUTS_ARE_ENABLED, MOE_CAN_BE_SET_ONLY_BY_SOFTWARE,
                                               BREAK_INPUT_BRK_IS_ACTIVE_HIGH, BREAK_INPUTS_DISABLED, WHEN_INACTIVE_OUTPUTS_DISABLED, WHEN_INACTIVE_DISABLED, LOCK_OFF, 0);
    }
}

void set_generation_timing(uint32_t tmr_freq, uint16_t frq, uint8_t duty)
{
    uint32_t timer_arr = tmr_freq / frq;
    uint32_t timer_main_channel = (timer_arr / 100) * duty;
    uint32_t end_coil_reply_sampling = (timer_arr - 10);
    uint32_t start_coil_toque_sampling = timer_main_channel / 2;
    coil_frequency_timer.set_timer_config(timer_main_channel, timer_main_channel, start_coil_toque_sampling, end_coil_reply_sampling, timer_arr, ((main_frq / 1000000) - 1), 0);
}

extern "C" void TIM3_IRQHandler(void)
{
    {
        if (TIM3->SR & TIM_SR_CC3IF_Msk)
        {
            /* Начало замера тока катушки */
            act_coil_current = ADC2->DR;
        }
        if (TIM3->SR & TIM_SR_CC4IF_Msk)
        {
            /* Конец ответа катушки */
            if (TIM2->SR & TIM_SR_CC1IF_Msk)
                new_signal = TIM2->CCR1;
            TIM2->CR1 &= ~TIM_CR1_CEN_Msk;
            TIM2->SR = ~TIM2->SR;
            TIM2->CNT = 0;
        }
    }
    TIM3->SR = ~TIM3->SR;
}