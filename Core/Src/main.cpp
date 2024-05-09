#include <main.h>
#define ALPHA_SMOOTH_VALUE (0.2)
#define COIL_CURRENT_SHUNT (0.2)

/* Ноги проца */
GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);
GPIO btn_2(GPIOB, 14U);
GPIO btn_1(GPIOB, 13U);
GPIO gen_freq(GPIOB, 5U);
GPIO buzz_freq(GPIOB, 9U);
GPIO usb_tx(GPIOA, 9U);
GPIO usb_rx(GPIOA, 10U);
GPIO cur_fault(GPIOC, 13U);
GPIO bat_voltage_pin(GPIOA, 4U);
GPIO coil_current_pin(GPIOA, 2U);
GPIO coil_response(GPIOA, 1U);
GPIO dc_enable(GPIOB, 3);
GPIO dc_check(GPIOA, 3U);

/* Таймера на генерацию и сэмплирование */
uint16_t cur_fault_delay = 0;
timer buzzer_timer(TIM4);
timer coil_frequency_timer(TIM3);
timer sampling_timer(TIM1);

/* Библиотека логирования и настройка уарт`а */
uint8_t usb_buffer[USB_BUFFER_SIZE]{0};
usart usb_line(USART1);
SimpleLog Logger(log_out_method);

/* ДМА на сэмплирование и уарт */
dma_control adc_samling_dma(DMA1, DMA1_Channel1);
dma_control usb_tx_dma(DMA1, DMA1_Channel4);
dma_control usb_rx_dma(DMA1, DMA1_Channel5);

/* ModBus */
uint16_t usInputRegisters[MB_INPUT_ADR_MAX] = {0};
uint16_t usHoldingRegisters[MB_HOLDING_ADR_MAX] = {0};
ModBusRTU ModBus(ModBusTxCallback, &usInputRegisters[0], &usHoldingRegisters[0]);

void system_monitor();

int main(void)
{
  /**
   * @brief   Запуск проца для проверки напряжений
   * @details Частота 125КГц
   */
  clock_control::set_ahb_prescaler(clock_control::AHB_PRESCALER_Type::SYSCLK_DIVIDED_BY_64);
  gen_freq.clock_enable(true);
  led_pin.clock_enable(true);
  dc_enable.clock_enable(true);
  dc_check.clock_enable(true);
  bat_voltage_pin.clock_enable(true);

  AFIO->MAPR |= (AFIO_MAPR_SWJ_CFG_JTAGDISABLE);
  dc_enable.set_config(GPIO::output_push_pull);
  gen_freq.set_config(GPIO::output_push_pull);
  led_pin.set_config(GPIO::output_push_pull);
  dc_check.set_config(GPIO::input_analog);
  bat_voltage_pin.set_config(GPIO::input_analog);

  gen_freq.reset();
#if INVERT_GENERATOR_SIGNAL
  gen_freq.set();
#endif

  /**
   * @brief   Проверка напряжений и запуск dc-dc
   *
   * @details Проверка напряжения батареи, за которой следует
   *          запуск преобразователя и последующая его проверка
   * @note    При запуске преобразователя происходит просадка напряжения,
   *          для ее минимизации его запуск инициируется когда потребление
   *          процессора минимально
   */
  {
    dc_startup = 2000;
    dc_enable.set();
    uint16_t core_voltage;
    if (get_core_voltage(&core_voltage) && adc_start_system_monitor(core_voltage))
    {
      SysTick_Config(125);
      NVIC_EnableIRQ(SysTick_IRQn);

      while (dc_startup)
      {
        if (ADC_END_INJ_CONVERSION(ADC2))
        {
          ADC_CLEAR_STATUS(ADC2);
          usInputRegisters[INPUT_REG_REF_VOLTAGE] = core_voltage;
          usInputRegisters[INPUT_REG_BAT_VOLTAGE] = get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC2->JDR1), 10000, 5100);
          usInputRegisters[INPUT_REG_DC_VOLTAGE] = get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC2->JDR2), 1000, 100);
        }
        ADC_INJ_START(ADC2);
      }
    }
    else
      NVIC_SystemReset();
  }

  /* конфижим тактирование проца */
  /**
   * @brief   конфижим тактирование проца
   *
   * @details Установка максимальной частоты,
   *          включить проверку внешнего источника тактирования(CSS),
   *          если он вышел из строя - перезагрузка.
   */
  if (clock_control::hse::enable(true) && clock_control::hse::ready())
  {
    clock_control::pll::hse_clock_divided(false);
    clock_control::pll::pll_clock_source(clock_control::PLL_CLOCK_SOURCE_Type::HSE_oscillator);
    clock_control::pll::multiplication_factor(clock_control::MULTIPLICATION_FACTOR_Type::PLL_INPUT_CLOCK_X7);
    if (clock_control::pll::enable(true) && clock_control::pll::ready())
    {
      FLASH->ACR |= (0x02 << FLASH_ACR_LATENCY_Pos);
      clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK);
      if (clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK) && clock_control::hse::enable_security_system(true))
      {
        clock_control::set_ahb_prescaler(clock_control::AHB_PRESCALER_Type::SYSCLK_NOT_DIVIDED);
        clock_control::set_apb1_prescaler(clock_control::APB1_PRESCALER_Type::HCLK_DIVIDED_BY_2);
        clock_control::set_apb2_prescaler(clock_control::APB2_PRESCALER_Type::HCLK_NOT_DIVIDED);
        clock_control::set_adc_prescaler(clock_control::ADC_PRESCALER_Type::PCLK2_DIVIDED_BY_4);
        goto start_system;
      }
    }
  }
  led_pin.reset();
  NVIC_SystemReset();

start_system:
  /* конфижим ноги проца - уарт */
  usb_rx.clock_enable(true);
  usb_rx.set_config(GPIO::alternate_push_pull, GPIO::alternate_input_pull_up);
  usb_tx.clock_enable(true);
  usb_tx.set_config(GPIO::alternate_push_pull, GPIO::alternate_output_mode);
  /* конфижим ноги проца - ток */
  coil_current_pin.clock_enable(true);
  coil_current_pin.set_config(GPIO::input_analog);
  /* конфижим ноги проца - кнопки */
  btn_3.clock_enable(true);
  btn_3.set_config(GPIO::input_floating);
  btn_1.clock_enable(true);
  btn_1.set_config(GPIO::input_floating);
  btn_2.clock_enable(true);
  btn_2.set_config(GPIO::input_floating);
  /* конфижим ноги проца - ответ катушки */
  coil_response.clock_enable(true);
  coil_response.set_config(GPIO::input_analog);
  /* конфижим ноги проца - динамик */
  buzz_freq.clock_enable(true);
  buzz_freq.set_config(GPIO::alternate_push_pull, GPIO::alternate_output_mode);

  /* Конфижим УАРТ в дма режим */
  usb_line.usart_config(NUMBER_OF_DATA_BITS_IS_8, PARITY_CONTROL_DISABLED, NUMBER_OF_STOP_BIT_IS_1, DMA_MODE_RXEN_TXEN, 56000000, 9600);
  usb_line.interrupt_config(USART_CR1_IDLEIE_Msk);
  set_usb_tx_dma_cfg();
  set_usb_rx_dma_cfg();
  NVIC_SetPriority(DMA1_Channel4_IRQn, 3);
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  NVIC_SetPriority(DMA1_Channel5_IRQn, 3);
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  NVIC_SetPriority(USART1_IRQn, 4);
  NVIC_EnableIRQ(USART1_IRQn);

  /* превышение тока на катушке */
  cur_fault.clock_enable(true);
  cur_fault.set_config(GPIO::input_pull_up);
  EXTI->IMR |= (EXTI_IMR_MR13_Msk);
  EXTI->EMR |= (EXTI_EMR_MR13_Msk);
  EXTI->FTSR |= (EXTI_FTSR_TR13_Msk);
  AFIO->EXTICR[3] |= (0b0010 << AFIO_EXTICR4_EXTI13_Pos);
  EXTI->PR = EXTI->PR;
  NVIC_SetPriority(EXTI15_10_IRQn, 0);
  NVIC_EnableIRQ(EXTI15_10_IRQn);

  Logger.LogI((char *)"Starting!\n\r");
  uint16_t core_voltage;
  if (get_core_voltage(&core_voltage) && adc_start_system_monitor(core_voltage))
  {
    usInputRegisters[INPUT_REG_REF_VOLTAGE] = core_voltage;
    NVIC_EnableIRQ(ADC1_2_IRQn);
    ADC_START(ADC2);
    SysTick_Config(56000);
    NVIC_EnableIRQ(SysTick_IRQn);

    {
      if (adc::enable(ADC1))
      {
        // конфигурация
        adc::set_cr1_config(ADC1, AWDEN__REGULAR_CHANNELS_ANALOG_WATCHDOG_DISABLED, JAWDEN__INJECTED_CHANNELS_ANALOG_WATCHDOG_DISABLED,
                            DUALMOD__INDEPENDENT_MODE, 0,
                            JDISCEN__INJECTED_CHANNELS_DISCONTINUOUS_MODE_DISABLED, DISCEN__REGULAR_CHANNELS_DISCONTINUOUS_MODE_DISABLED,
                            JAUTO__AUTOMATIC_INJECTED_CONVERSION_DISABLED,
                            AWDSGL__ANALOG_WATCHDOG_ON_SINGLE_CHANNEL, SCAN__SCAN_MODE_DISABLED,
                            JEOCIE__JEOC_INTERRUPT_DISABLED, AWDIE__ANALOG_WATCHDOG_INTERRUPT_DISABLED, EOCIE__EOC_INTERRUPT_DISABLED, 0);

        adc::set_cr2_config(ADC1, TSVREFE__TEMPERATURE_SENSOR_VREFINT_CHANNEL_DISABLED,
                            EXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED, EXTSEL__TIMER_1_CC1_EVENT,
                            JEXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED, JEXTSEL__JSWSTART,
                            ALIGN__RIGHT_ALIGNMENT, DMA__DMA_MODE_ENABLED, RSTCAL__CALIBRATION_REGISTER_INITIALIZED,
                            CONT__SINGLE_CONVERSION_MODE, ADON__ENABLE_ADC);

        adc::set_sampling(ADC1, 1, SMP_1_5_cycles);
        adc::set_regular_sequence(ADC1, 0, 1, 1);
      }

      adc_samling_dma.dma_set_config(MEM2MEM_Disabled, PL_Low,
                                     MSIZE_16bits, PSIZE_16bits,
                                     MINC_Enabled, PINC_Disabled, CIRC_Disabled, Read_From_Peripheral,
                                     TEIE_Disabled, HTIE_Disabled, TCIE_Disabled);
    }

    set_timer_config();
    // NVIC_EnableIRQ(TIM1_CC_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);
  }

  while (true)
  {
    system_monitor();

    if (!(btn_2.get_level()))
      buzzer_timer.set_timer_config(0, 0, 0, 1, 454, 71, 0);

    if (usHoldingRegisters[0])
    {
      usHoldingRegisters[0] = 0;
      GPIOB->CRL &= ~(GPIO_CRL_CNF5_Msk);
      GPIOB->CRL |= (GPIO_CRL_MODE5_Msk);
#if INVERT_GENERATOR_SIGNAL
      GPIOB->BSRR = (GPIO_BSRR_BS5_Msk);
#else
      GPIOB->BRR = (GPIO_BRR_BR5_Msk);
#endif
      TIM3->CR1 &= ~TIM_CR1_CEN_Msk;
    }
  }
}

void system_monitor()
{
  if (ADC_END_INJ_CONVERSION(ADC2))
  {
    ADC_CLEAR_STATUS(ADC2);
    usInputRegisters[INPUT_REG_BAT_VOLTAGE] =
        smooth_value(ALPHA_SMOOTH_VALUE, get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC2->JDR1), 10000, 5100), usInputRegisters[INPUT_REG_BAT_VOLTAGE]);
    usInputRegisters[INPUT_REG_DC_VOLTAGE] =
        smooth_value(ALPHA_SMOOTH_VALUE, get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC2->JDR2), 1000, 100), usInputRegisters[INPUT_REG_DC_VOLTAGE]);

    if (system_monitor_handler(usInputRegisters[INPUT_REG_REF_VOLTAGE], usInputRegisters[INPUT_REG_BAT_VOLTAGE], usInputRegisters[INPUT_REG_DC_VOLTAGE]) != SYSTEM_OK)
      NVIC_SystemReset();
  }
}