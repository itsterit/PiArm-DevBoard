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

  led_pin.set();
  for (uint32_t StartDelay = 0; StartDelay < 0xFFFFF; StartDelay++)
    asm("NOP");
  led_pin.reset();

  clk_out.clock_enable(true);
  clk_out.set_config(GPIO::alternate_push_pull);
  RCC->CFGR |= RCC_CFGR_MCOSEL_SYSCLK;

  clock_control::hse::enable(true);
  clock_control::hse::clock_ready();
  if (clock_control::clock_switch(clock_control::HSE_SELECTED_AS_SYSTEM_CLOCK))
  {
    clock_control::hse::enable_security_system(true);
    led_pin.set();
  }

  while (true)
  {

    if (led_pin.get_level())
      led_pin.reset();
    else
      led_pin.set();

    for (uint32_t StartDelay = 0; StartDelay < 0xFFFF; StartDelay++)
      asm("NOP");
  }
}

extern "C" void RCC_IRQHandler(void)
{
  led_pin.set();
}