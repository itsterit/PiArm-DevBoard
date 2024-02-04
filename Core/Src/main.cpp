#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"

GPIO led_pin(GPIOB, 11U);
GPIO gen_freq(GPIOB, 5U);

int main(void)
{
  gen_freq.clock_enable(true);
  gen_freq.set_config(GPIO::output_open_drain);
  GPIOB->ODR &= ~(0b01 << GPIO_ODR_ODR5_Pos);

  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);
  GPIOB->ODR |= (0b01 << GPIO_ODR_ODR11_Pos);

  while (true)
  {
    asm("NOP");
  }
}
