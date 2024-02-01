#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{

  /**
   * @brief Отладочный светодиод
   *        22 пин - PB11
   */
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  GPIOB->CRH &= ~(GPIO_CRH_CNF11_Msk); 
  GPIOB->CRH |= (0b11 << GPIO_CRH_MODE11_Pos); 
  GPIOB->ODR |= (0b01 << GPIO_ODR_ODR11_Pos);

  /* Loop forever */
  while (true)
  {
    asm("NOP");
  }
}
