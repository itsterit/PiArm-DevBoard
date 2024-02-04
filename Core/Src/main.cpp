#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"

GPIO led_pin(GPIOB, 11);

int main(void)
{

  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);

  GPIOB->ODR |= (0b01 << GPIO_ODR_ODR11_Pos);

  while (true)
  {
    asm("NOP");
  }
}
