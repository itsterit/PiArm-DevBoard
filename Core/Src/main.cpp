#include <main.h>
#define ALPHA_SMOOTH_VALUE (0.3)
#define COIL_CURRENT_SHUNT (0.2)

/* Ноги проца */
GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);
GPIO btn_2(GPIOB, 14U);
GPIO btn_1(GPIOB, 13U);
GPIO gen_freq(GPIOB, 5U);
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
        if (ADC1->SR & ADC_SR_JEOS_Msk)
        {
          ADC_CLEAR_STATUS(ADC1);
          usInputRegisters[INPUT_REG_REF_VOLTAGE] = get_adc_ref_voltage(ADC1->JDR1);
          usInputRegisters[INPUT_REG_BAT_VOLTAGE] = get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC1->JDR2), 10000, 5100);
          usInputRegisters[INPUT_REG_DC_VOLTAGE] = get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC1->JDR3), 1000, 100);
        }
        ADC_INJ_START(ADC1);
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
    clock_control::pll::multiplication_factor(clock_control::MULTIPLICATION_FACTOR_Type::PLL_INPUT_CLOCK_X9);
    if (clock_control::pll::enable(true) && clock_control::pll::ready())
    {
      FLASH->ACR |= (0x02 << FLASH_ACR_LATENCY_Pos);
      clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK);
      if (clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK) && clock_control::hse::enable_security_system(true))
      {
        clock_control::set_ahb_prescaler(clock_control::AHB_PRESCALER_Type::SYSCLK_NOT_DIVIDED);
        clock_control::set_apb1_prescaler(clock_control::APB1_PRESCALER_Type::HCLK_DIVIDED_BY_2);
        clock_control::set_apb2_prescaler(clock_control::APB2_PRESCALER_Type::HCLK_NOT_DIVIDED);
        clock_control::set_adc_prescaler(clock_control::ADC_PRESCALER_Type::PCLK2_DIVIDED_BY_6);
        goto start_system;
      }
    }
  }
  led_pin.reset();
  NVIC_SystemReset();

start_system:
  /* конфижим ноги проца */
  usb_tx.clock_enable(true);
  coil_current_pin.clock_enable(true);
  usb_rx.clock_enable(true);
  btn_3.clock_enable(true);
  btn_1.clock_enable(true);
  coil_response.clock_enable(true);
  btn_2.clock_enable(true);
  usb_tx.set_config(GPIO::alternate_push_pull, GPIO::alternate_output_mode);
  usb_rx.set_config(GPIO::alternate_push_pull, GPIO::alternate_input_pull_up);
  btn_3.set_config(GPIO::input_floating);
  btn_1.set_config(GPIO::input_floating);
  coil_current_pin.set_config(GPIO::input_analog);
  coil_response.set_config(GPIO::input_analog);
  btn_2.set_config(GPIO::input_floating);

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

  adc_start_system_monitor(usInputRegisters[INPUT_REG_REF_VOLTAGE]);
  NVIC_EnableIRQ(ADC1_2_IRQn);
  ADC_START(ADC1);

  SysTick_Config(72000);
  NVIC_EnableIRQ(SysTick_IRQn);

  set_timer_config();
  NVIC_EnableIRQ(TIM1_CC_IRQn);
  NVIC_EnableIRQ(TIM3_IRQn);

  while (true)
  {
    if (!(btn_2.get_level()))
      NVIC_SystemReset();

    if (ADC1->SR & ADC_SR_JEOS_Msk)
    {
      ADC_CLEAR_STATUS(ADC1);
      usInputRegisters[INPUT_REG_REF_VOLTAGE] =
          smooth_value(ALPHA_SMOOTH_VALUE, get_adc_ref_voltage(ADC1->JDR1), usInputRegisters[INPUT_REG_REF_VOLTAGE]);
      usInputRegisters[INPUT_REG_BAT_VOLTAGE] =
          smooth_value(ALPHA_SMOOTH_VALUE, get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC1->JDR2), 10000, 5100), usInputRegisters[INPUT_REG_BAT_VOLTAGE]);
      usInputRegisters[INPUT_REG_DC_VOLTAGE] =
          smooth_value(ALPHA_SMOOTH_VALUE, get_voltage_divider_uin(get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], ADC1->JDR3), 1000, 100), usInputRegisters[INPUT_REG_DC_VOLTAGE]);
      usInputRegisters[INPUT_REG_COIL_CUR] =
          smooth_value(ALPHA_SMOOTH_VALUE, (get_adc_voltage(usInputRegisters[INPUT_REG_REF_VOLTAGE], coil_current)), usInputRegisters[INPUT_REG_COIL_CUR]);
      if (system_monitor_handler(usInputRegisters[INPUT_REG_REF_VOLTAGE], usInputRegisters[INPUT_REG_BAT_VOLTAGE], usInputRegisters[INPUT_REG_DC_VOLTAGE]) != SYSTEM_OK)
        NVIC_SystemReset();
    }
  }
}