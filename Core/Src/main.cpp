#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"
#include "clock_control/clock_control.h"

GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);
GPIO btn_2(GPIOB, 14U);
GPIO btn_1(GPIOB, 13U);
GPIO clk_out(GPIOA, 8U);

int main(void)
{
  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);

  btn_3.clock_enable(true);
  btn_3.set_config(GPIO::input_floating);
  btn_2.clock_enable(true);
  btn_2.set_config(GPIO::input_floating);
  btn_1.clock_enable(true);
  btn_1.set_config(GPIO::input_floating);

  RCC->CR = 0x00;
  RCC->CR |= (0b00 << RCC_CR_PLLRDY_Pos);
  RCC->CR |= (0b00 << RCC_CR_PLLON_Pos);
  RCC->CR |= (0b00 << RCC_CR_CSSON_Pos);
  RCC->CR |= (0b00 << RCC_CR_HSEBYP_Pos);
  RCC->CR |= (0b00 << RCC_CR_HSERDY_Pos);
  RCC->CR |= (0b00 << RCC_CR_HSEON_Pos);
  RCC->CR |= (0b00 << RCC_CR_HSITRIM_Msk);
  RCC->CR |= (0b00 << RCC_CR_HSIRDY_Pos);
  RCC->CR |= (0b00 << RCC_CR_HSION_Pos);

  RCC->CFGR = 0x00;
  RCC->CFGR |= (0b00 << RCC_CFGR_USBPRE_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_PLLMULL_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_PLLXTPRE_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_PLLSRC_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_ADCPRE_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_PPRE2_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_PPRE1_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_HPRE_Pos);
  RCC->CFGR |= (0b00 << RCC_CFGR_SW_Pos);

  clk_out.clock_enable(true);
  clk_out.set_config(GPIO::alternate_push_pull);
  RCC->CFGR |= RCC_CFGR_MCOSEL_PLL_DIV2;

  while (true)
  {
  }
}