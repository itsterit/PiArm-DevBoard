#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"

GPIO led_pin(GPIOB, 11);

int main(void)
{

  if (led_pin.clock_enable(true))
  {
    GPIOB->CRH &= ~(GPIO_CRH_CNF11_Msk);
    GPIOB->CRH |= (0b11 << GPIO_CRH_MODE11_Pos);
    GPIOB->ODR |= (0b01 << GPIO_ODR_ODR11_Pos);
  }

  while (true)
  {
    asm("NOP");
  }
}