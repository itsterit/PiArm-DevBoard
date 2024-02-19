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

  clock_control::hsi::enable(true);
  clock_control::hse::enable(true);
  clock_control::pll::enable(false);
  if (clock_control::hsi::ready)
  {
    clock_control::pll::pll_clock_source(clock_control::PLL_CLOCK_SOURCE_Type::HSI_oscillator_divided_by_2);
    clock_control::pll::multiplication_factor(clock_control::MULTIPLICATION_FACTOR_Type::PLL_INPUT_CLOCK_X9);
    clock_control::pll::enable(true);
    if (clock_control::pll::ready())
    {
      if (clock_control::clock_switch(clock_control::PLL_SELECTED_AS_SYSTEM_CLOCK))
      {
        led_pin.set();
      }
    }
  }
  else
  {
    NVIC_SystemReset();
  }

  clk_out.clock_enable(true);
  clk_out.set_config(GPIO::alternate_push_pull);
  RCC->CFGR |= RCC_CFGR_MCOSEL_PLL_DIV2;

  while (true)
  {
  }
}

extern "C" void RCC_IRQHandler(void)
{
  led_pin.set_config(GPIO::output_push_pull);
  led_pin.set();
}
