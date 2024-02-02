#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"

GPIO led_pin(GPIOB, 11);

int main(void)
{

  led_pin.clock_enable(true);
  // RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  GPIOB->CRH &= ~(GPIO_CRH_CNF11_Msk); 
  GPIOB->CRH |= (0b11 << GPIO_CRH_MODE11_Pos); 
  GPIOB->ODR |= (0b01 << GPIO_ODR_ODR11_Pos);

  /* Loop forever */
  while (true)
  {
    asm("NOP");
  }
}

// 0x40000000+ 0x00010000 + 0x00000800 =  0x40010800 //a
// 0x40000000 + 0x00010000 + 0x00000C00 = 0x40010C00 //b
