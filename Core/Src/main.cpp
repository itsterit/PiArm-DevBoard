#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"
#include "clock_control/clock_control.h"
#include "timer/timer.h"
#include "usart/usart.h"
#include "SimpleLog/SimpleLog.h"

GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);
GPIO btn_2(GPIOB, 14U);
GPIO btn_1(GPIOB, 13U);
GPIO gen_freq(GPIOB, 5U);

GPIO usb_tx(GPIOA, 9U);
GPIO usb_rx(GPIOA, 10U);

timer coil_frequency_timer(TIM3);
timer sampling_timer(TIM1);

usart usb_line(USART1);
void log_out_method(char *str, uint8_t len);
SimpleLog Logger(log_out_method);

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

int main(void)
{
  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);

  usb_tx.clock_enable(true);
  usb_tx.set_config(GPIO::alternate_push_pull);
  usb_rx.clock_enable(true);
  usb_rx.set_config(GPIO::alternate_push_pull);

  gen_freq.clock_enable(true);
  gen_freq.set_config(GPIO::alternate_push_pull);

  btn_3.clock_enable(true);
  btn_3.set_config(GPIO::input_floating);
  btn_2.clock_enable(true);
  btn_2.set_config(GPIO::input_floating);
  btn_1.clock_enable(true);
  btn_1.set_config(GPIO::input_floating);

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

  sampling_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_ENABLE, 0, 0);
  sampling_timer.slave_mode_control(INTERNAL_TRIGGER2, TRIGGER_MODE);
  sampling_timer.set_timer_config(0, 0, 0, 0, 5, 72, 0);
  sampling_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_DISABLE, COUNTER_DISABLE);
  // NVIC_EnableIRQ(TIM1_UP_IRQn);

  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP_Msk;
  AFIO->MAPR |= (0b01 << AFIO_MAPR_TIM3_REMAP_PARTIALREMAP_Pos);
  coil_frequency_timer.set_channel_output_config(2U, OUTPUT_COMPARE_CLEAR_DISABLE, OUTPUT_COMPARE_PRELOAD_ENABLE, OUTPUT_COMPARE_FAST_ENABLE, CHANNEL_PWM_MODE_1);
  coil_frequency_timer.set_event_generation(TRIGGER_GENERATION_DISABLE, UPDATE_GENERATION_DISABLE, 0);
  coil_frequency_timer.set_dma_interrupt_config(TRIGGER_DMA_REQUEST_DISABLE, UPDATE_DMA_REQUEST_DISABLE, TRIGGER_INTERRUPT_DISABLE, UPDATE_INTERRUPT_ENABLE, 0, (TIM_DIER_CC2IE_Msk | TIM_DIER_CC3IE_Msk | TIM_DIER_CC4IE_Msk));
  coil_frequency_timer.slave_mode_control(INTERNAL_TRIGGER0, SLAVE_MODE_DISABLED);
  coil_frequency_timer.master_mode_config(MASTER_MODE_COMPARE_PULSE);
  coil_frequency_timer.capture_compare_register(0, TIM_CCER_CC2E_Msk);
  config_timer(1000000, 5000, 20);
  coil_frequency_timer.set_counter_config(ARR_REGISTER_BUFFERED, COUNTER_UPCOUNTER, ONE_PULSE_DISABLE, COUNTER_ENABLE);
  // NVIC_EnableIRQ(TIM3_IRQn);

  while (1)
  {
    led_pin.set();
    if ((btn_2.get_level()))
    {
      led_pin.reset();
      break;
    }
  }

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // разрешаем тактирование порта GPIOA

  // настройка вывода PA9 (TX1) на режим альтернативной функции с активным выходом
  // Биты CNF = 10, ,биты MODE = X1
  GPIOA->CRH &= (~GPIO_CRH_CNF9_0);
  GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9);

  // настройка вывода PA10 (RX1) на режим входа с подтягивающим резистором
  // Биты CNF = 10, ,биты MODE = 00, ODR = 1
  GPIOA->CRH &= (~GPIO_CRH_CNF10_0);
  GPIOA->CRH |= GPIO_CRH_CNF10_1;
  GPIOA->CRH &= (~(GPIO_CRH_MODE10));
  GPIOA->BSRR |= GPIO_ODR_ODR10;

  usb_line.usart_config(NUMBER_OF_DATA_BITS_IS_8, PARITY_CONTROL_DISABLED, NUMBER_OF_STOP_BIT_IS_1, 72000000, 9600);
  Logger.LogV((char *)"\n\rStarting...\n\r");
  // usb_line.transmit((uint8_t *)"~", 1);

  USART1->CR1 |= (USART_CR1_RXNEIE_Msk);
  NVIC_EnableIRQ(USART1_IRQn);

  while (true)
  {
    if (!(btn_2.get_level()))
      NVIC_SystemReset();
  }
}

extern "C" void USART1_IRQHandler(void)
{
  USART1->SR = ~USART1->SR;
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BRR = (0b01 << 11U);

  char msg = USART1->DR;
  USART1->DR = msg;
}

extern "C" void TIM1_UP_IRQHandler(void)
{
  TIM1->SR = ~TIM1->SR;

  // GPIOB->BSRR = (0b01 << 11U);
  // GPIOB->BSRR = (0b01 << 11U);
  // GPIOB->BSRR = (0b01 << 11U);
  // GPIOB->BSRR = (0b01 << 11U);
  // GPIOB->BRR = (0b01 << 11U);
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

void log_out_method(char *str, uint8_t len)
{
  usb_line.transmit((uint8_t *)str, len);
}