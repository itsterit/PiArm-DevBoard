#include <main.h>

GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);
GPIO btn_2(GPIOB, 14U);
GPIO btn_1(GPIOB, 13U);
GPIO gen_freq(GPIOB, 5U);
GPIO usb_tx(GPIOA, 9U);
GPIO usb_rx(GPIOA, 10U);
GPIO cur_fault(GPIOC, 13U);
GPIO dc_enable(GPIOB, 3);
GPIO bat_voltage_pin(GPIOA, 4U);
GPIO coil_current_pin(GPIOA, 2U);
GPIO coil_response(GPIOA, 1U);
GPIO dc_check(GPIOA, 3U);

uint16_t reboot_delay = 0;
uint16_t cur_fault_delay = 0;
timer coil_frequency_timer(TIM3);
timer sampling_timer(TIM1);

uint8_t usb_buffer[USB_BUFFER_SIZE]{0};
usart usb_line(USART1);
SimpleLog Logger(log_out_method);

dma_control adc_samling_dma(DMA1, DMA1_Channel1);
dma_control usb_tx_dma(DMA1, DMA1_Channel4);
dma_control usb_rx_dma(DMA1, DMA1_Channel5);

uint16_t usInputRegisters[MB_INPUT_ADR_MAX] = {0};
uint16_t usHoldingRegisters[MB_HOLDING_ADR_MAX] = {0};
ModBusRTU ModBus(ModBusTxCallback, &usInputRegisters[0], &usHoldingRegisters[0]);

uint16_t ref_voltage = 0;
uint16_t voltage = 0;
int main(void)
{
  gen_freq.clock_enable(true);
  gen_freq.set_config(GPIO::output_push_pull);
  gen_freq.set();

  /* конфижим ноги проца */
  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);
  AFIO->MAPR |= (AFIO_MAPR_SWJ_CFG_JTAGDISABLE);
  dc_enable.clock_enable(true);
  dc_enable.set_config(GPIO::output_push_pull);
  usb_tx.clock_enable(true);
  usb_tx.set_config(GPIO::alternate_push_pull, GPIO::alternate_output_mode);
  usb_rx.clock_enable(true);
  usb_rx.set_config(GPIO::alternate_push_pull, GPIO::alternate_input_pull_up);
  btn_3.clock_enable(true);
  btn_3.set_config(GPIO::input_floating);
  btn_2.clock_enable(true);
  btn_2.set_config(GPIO::input_floating);
  btn_1.clock_enable(true);
  btn_1.set_config(GPIO::input_floating);
  bat_voltage_pin.clock_enable(true);
  bat_voltage_pin.set_config(GPIO::input_analog);
  coil_current_pin.clock_enable(true);
  coil_current_pin.set_config(GPIO::input_analog);
  coil_response.clock_enable(true);
  coil_response.set_config(GPIO::input_analog);
  dc_check.clock_enable(true);
  dc_check.set_config(GPIO::input_analog);

  dc_enable.set();

  /* конфижим тактирование проца */
  clock_control::hse::enable(true);
  if (clock_control::hse::ready())
  {
    clock_control::pll::hse_clock_divided(false);
    clock_control::pll::pll_clock_source(clock_control::PLL_CLOCK_SOURCE_Type::HSE_oscillator);
    clock_control::pll::multiplication_factor(clock_control::MULTIPLICATION_FACTOR_Type::PLL_INPUT_CLOCK_X9);
    clock_control::pll::enable(true);
    if (clock_control::pll::ready())
    {
      FLASH->ACR |= (0x02 << FLASH_ACR_LATENCY_Pos);
      clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK);
      if (clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK))
      {
        clock_control::set_ahb_prescaler(clock_control::AHB_PRESCALER_Type::SYSCLK_NOT_DIVIDED);
        clock_control::set_apb1_prescaler(clock_control::APB1_PRESCALER_Type::HCLK_DIVIDED_BY_2);
        clock_control::set_apb2_prescaler(clock_control::APB2_PRESCALER_Type::HCLK_NOT_DIVIDED);
        clock_control::set_adc_prescaler(clock_control::ADC_PRESCALER_Type::PCLK2_DIVIDED_BY_6);
      }
    }
  }

  /* Конфижим УАРТ в дма режим */
  usb_line.usart_config(NUMBER_OF_DATA_BITS_IS_8, PARITY_CONTROL_DISABLED, NUMBER_OF_STOP_BIT_IS_1, DMA_MODE_RXEN_TXEN, 72000000, 9600);
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

  Logger.LogI((char *)"\n\r--Starting--\n\r");

  get_core_voltage(&ref_voltage);
  get_voltage(&voltage, 3, ref_voltage);
  Logger.LogD((char *)"core voltage: %d\n\r", ref_voltage);
  Logger.LogD((char *)"Dc voltage:   %d\n\r",
              get_voltage_divider_uin(
                  (uint16_t)(ADC1->DR * (float)((float)ref_voltage / 4096)),
                  500,
                  100));

  adc::enable(ADC1);
  adc::set_cr1_config(ADC1, AWDEN__REGULAR_CHANNELS_ANALOG_WATCHDOG_ENABLED, JAWDEN__INJECTED_CHANNELS_ANALOG_WATCHDOG_DISABLED,
                      DUALMOD__INDEPENDENT_MODE, 0, JDISCEN__INJECTED_CHANNELS_DISCONTINUOUS_MODE_DISABLED,
                      DISCEN__REGULAR_CHANNELS_DISCONTINUOUS_MODE_DISABLED, JAUTO__AUTOMATIC_INJECTED_CONVERSION_DISABLED,
                      AWDSGL__ANALOG_WATCHDOG_ON_SINGLE_CHANNEL, SCAN__SCAN_MODE_DISABLED,
                      JEOCIE__JEOC_INTERRUPT_DISABLED, AWDIE__ANALOG_WATCHDOG_INTERRUPT_ENABLED, EOCIE__EOC_INTERRUPT_DISABLED, 2);

  adc::set_cr2_config(ADC1, TSVREFE__TEMPERATURE_SENSOR_VREFINT_CHANNEL_ENABLED,
                      EXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED, EXTSEL__SWSTART,
                      JEXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED, JEXTSEL__JSWSTART,
                      ALIGN__RIGHT_ALIGNMENT, DMA__DMA_MODE_DISABLED, RSTCAL__CALIBRATION_REGISTER_INITIALIZED,
                      CONT__CONTINUOUS_CONVERSION_MODE, ADON__ENABLE_ADC);

  adc::set_analog_watchdog_threshold(ADC1,
                                     get_adc_code(ref_voltage, 100),
                                     0);
  adc::set_regular_sequence(ADC1, 0, 1, 2);
  adc::set_sampling(ADC1, 2, SMP_1_5_cycles);
  ADC_START(ADC1);

  // NVIC_EnableIRQ(TIM1_UP_IRQn);
  // NVIC_SetPriority(TIM1_CC_IRQn, 1);
  NVIC_EnableIRQ(TIM1_CC_IRQn);
  // NVIC_SetPriority(TIM3_IRQn, 2);
  NVIC_EnableIRQ(TIM3_IRQn);

  led_pin.set();
  cur_fault_delay = 6000;
  SysTick_Config(72000);
  NVIC_EnableIRQ(SysTick_IRQn);

  NVIC_EnableIRQ(ADC1_2_IRQn);

  while (true)
  {
    if (!(btn_2.get_level()))
      NVIC_SystemReset();
    // led_pin.reset();
  }
}

extern "C" void ADC1_2_IRQHandler(void)
{
  if (ADC1->SR & ADC_SR_AWD_Msk)
  {
    led_pin.set();
    led_pin.set();
    led_pin.set();
    led_pin.set();
    led_pin.set();
    led_pin.set();
    led_pin.set();
    led_pin.reset();
  }
  ADC1->SR = ~ADC1->SR;
  // Logger.LogD((char *)"ADC_DATA: %d\n\r", ADC_DATA(ADC1));
}
