#include "main.h"
#define SAMPLING_POINT_AMOUNT 100

void set_timer_config()
{
    check_system_parameters();
    gen_freq.clock_enable(true);
    gen_freq.set_config(GPIO::alternate_push_pull, GPIO::alternate_output_mode);
    AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP_Msk;
    AFIO->MAPR |= (0b01 << AFIO_MAPR_TIM3_REMAP_PARTIALREMAP_Pos);

    // конфигурация для сэмплирования
    {
        // RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        // TIM2->PSC = 56000 - 1; // новая частота 1Khz

        // TIM2->CCMR1 |= (0b01 << TIM_CCMR1_CC1S_Pos);         // выбираем TI4 для TIM5_CH4
        // TIM2->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC); // не фильтруем и делитель не используем

        // TIM2->CCMR1 |= (0b01 << TIM_CCMR1_CC2S_Pos);         // выбираем TI4 для TIM5_CH4
        // TIM2->CCMR1 &= ~(TIM_CCMR1_IC2F | TIM_CCMR1_IC2PSC); // не фильтруем и делитель не используем

        // TIM2->CCER &= ~TIM_CCER_CC1P; // выбираем захват по переднему фронту
        // TIM2->CCER |= TIM_CCER_CC1E;  // включаем режим захвата для 4-го канала

        // TIM2->CCER |= TIM_CCER_CC2P; // выбираем захват по заднему фронту
        // TIM2->CCER |= TIM_CCER_CC2E; // включаем режим захвата для 4-го канала

        // // TIM2->DIER |= TIM_DIER_CC1IE; // разрешаем прерывание по захвату
        // TIM2->DIER |= TIM_DIER_CC2IE; // разрешаем прерывание по захвату

        // TIM2->CR1 |= TIM_CR1_CEN; // включаем счётчик
    }

    // Таймер сэмплирования сигнала
    {
        sampling_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_ENABLE, 0, (TIM_DIER_UIE_Msk));
        sampling_timer.slave_mode_control(INTERNAL_TRIGGER2, TRIGGER_MODE);
        sampling_timer.set_timer_config(0, 0, 0, 0, 8000, 13, 0);
        sampling_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_ENABLE, COUNTER_DISABLE);
        sampling_timer.master_mode_config(MASTER_MODE_COMPARE_PULSE);
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
                                                      (0));
        coil_frequency_timer.slave_mode_control(INTERNAL_TRIGGER1, SLAVE_MODE_DISABLED);
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
                                       TEIE_Disabled, HTIE_Disabled, TCIE_Enabled);
        adc_samling_dma.dma_start(SAMPLING_POINT_AMOUNT, (uint32_t *)&usInputRegisters[10], (uint32_t *)&ADC1->DR);
    }

    // Звук
    {
        buzzer_timer.set_channel_output_config(4U, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_ENABLE, OUTPUT_COMPARE_FAST_ENABLE, CHANNEL_PWM_MODE_1);
        buzzer_timer.set_event_generation(TRIGGER_GENERATION_DISABLE, UPDATE_GENERATION_DISABLE, 0);
        buzzer_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_DISABLE, 0, 0);
        buzzer_timer.capture_compare_register(0, TIM_CCER_CC4E_Msk);
        buzzer_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_DISABLE, COUNTER_ENABLE);
        // buzzer_timer.set_timer_config(0, 0, 0, 1, 100, 55, 0);
    }
}

void set_generation_timing(uint32_t tmr_freq, uint16_t frq, uint8_t duty)
{
    uint32_t timer_arr = tmr_freq / frq;
    uint32_t timer_main_channel = (timer_arr / 100) * duty;
    uint32_t end_coil_reply_sampling = (timer_arr - 10);
    uint32_t start_coil_toque_sampling = timer_main_channel / 2;
    coil_frequency_timer.set_timer_config(timer_main_channel, timer_main_channel, start_coil_toque_sampling, end_coil_reply_sampling, timer_arr, 55, 0);
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
            // TIM1->CR1 &= ~(TIM_CR1_CEN_Msk);
            // TIM1->SR = ~TIM1->SR;
            // TIM1->CNT = 0;
        }
        if (TIM3->SR & TIM_SR_CC3IF_Msk)
        {
            /* Начало замера тока катушки */
            // TIM2->SR = ~TIM2->SR;
            // TIM2->CNT = 0;
            // TIM2->CR1 |= TIM_CR1_CEN;
            // act_coil_current = ADC2->DR;
        }
        if (TIM3->SR & TIM_SR_CC2IF_Msk)
        {
            GPIOB->BSRR = (0b01 << 11U);
            GPIOB->BRR = (0b01 << 11U);
        }
    }
    TIM3->SR = ~TIM3->SR;
}

extern "C" void DMA1_Channel1_IRQHandler(void)
{
    {
        // Если мы тут - значит данные получены
        TIM1->CR1 &= ~(TIM_CR1_CEN_Msk);
        TIM1->SR = ~TIM1->SR;
        TIM1->CNT = 0;
        usInputRegisters[9] = (100 - DMA1_Channel1->CNDTR);
        adc_samling_dma.dma_start(SAMPLING_POINT_AMOUNT, (uint32_t *)&usInputRegisters[10], (uint32_t *)&ADC1->DR);
    }
    DMA1->IFCR = DMA_IFCR_CGIF1_Msk;
}
