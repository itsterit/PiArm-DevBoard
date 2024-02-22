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
GPIO gen_freq(GPIOB, 5U);

int main(void)
{
  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);
  gen_freq.clock_enable(true);
  gen_freq.set_config(GPIO::output_push_pull);

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
    clock_control::pll::pll_clock_source(HSE_oscillator);
    clock_control::pll::multiplication_factor(PLL_INPUT_CLOCK_X9);
    clock_control::pll::enable(true);
    if (clock_control::pll::ready())
    {
      clock_control::set_apb1_prescaler(2);
      clock_control::set_apb2_prescaler(1);
      clock_control::set_adc_prescaler(6);

      FLASH->ACR |= (0x02 << FLASH_ACR_LATENCY_Pos);
      clock_control::clock_switch(PLL_SELECTED_AS_SYSTEM_CLOCK);
      if (clock_control::clock_switch(PLL_SELECTED_AS_SYSTEM_CLOCK))
      {
        clk_out.clock_enable(true);
        clk_out.set_config(GPIO::alternate_push_pull);
        RCC->CFGR |= RCC_CFGR_MCOSEL_HSE;
      }
    }
  }

  led_pin.set();
  while (true)
  {
    if (gen_freq.get_level())
      gen_freq.reset();
    else
      gen_freq.set();
  }
}
