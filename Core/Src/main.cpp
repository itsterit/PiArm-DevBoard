#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"

GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);

int main(void)
{
  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);

  btn_3.clock_enable(true);
  btn_3.set_config(GPIO::input_floating);
  
  led_pin.set();
  for(uint32_t StartDelay = 0; StartDelay < 0xFFFFF; StartDelay++)
      asm("NOP");
  led_pin.reset();

  while (true)
  {
    if( !btn_3.get_level() )
      led_pin.set();
    else
      led_pin.reset();


    asm("NOP");
  }
}
