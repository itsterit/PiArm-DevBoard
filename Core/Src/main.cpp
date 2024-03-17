#include <main.h>

GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);
GPIO btn_2(GPIOB, 14U);
GPIO btn_1(GPIOB, 13U);
GPIO gen_freq(GPIOB, 5U);
GPIO usb_tx(GPIOA, 9U);
GPIO usb_rx(GPIOA, 10U);
GPIO cur_fault(GPIOC, 13U);

GPIO bat_voltage_pin(GPIOA, 4U);

uint8_t cur_fault_delay = 0;
timer coil_frequency_timer(TIM3);
timer sampling_timer(TIM1);

uint8_t usb_buffer[USB_BUFFER_SIZE]{0};
usart usb_line(USART1);
SimpleLog Logger(log_out_method);
dma_control usb_tx_dma(DMA1, DMA1_Channel4);
dma_control usb_rx_dma(DMA1, DMA1_Channel5);

uint16_t usInputRegisters[MB_INPUT_ADR_MAX] = {0};
uint16_t usHoldingRegisters[MB_HOLDING_ADR_MAX] = {0};
ModBusRTU ModBus(ModBusTxCallback, &usInputRegisters[0], &usHoldingRegisters[0]);

int main(void)
{
  /* конфижим ноги проца */
  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);
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

  SysTick_Config(72000);
  NVIC_EnableIRQ(SysTick_IRQn);

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

  /* таймер настройки сэмплирования и настройка задающего таймер */
  set_timer_config();
  // NVIC_EnableIRQ(TIM1_UP_IRQn);
  // NVIC_EnableIRQ(TIM3_IRQn);

  while (true)
  {
    adc_set_config();

    if (!(btn_2.get_level()))
      NVIC_SystemReset();

    if ((usHoldingRegisters[0] && usHoldingRegisters[1]))
    {
      set_generation_timing(1000000, usHoldingRegisters[0], usHoldingRegisters[1]);
      usHoldingRegisters[0] = 0;
      usHoldingRegisters[1] = 0;
    }
  }
}
