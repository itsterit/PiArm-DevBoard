#include <stdint.h>
#include <main.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"
#include "clock_control/clock_control.h"

GPIO led_pin(GPIOB, 11U);
GPIO btn_3(GPIOB, 15U);
GPIO btn_2(GPIOB, 14U);
GPIO btn_1(GPIOB, 13U);
GPIO gen_freq(GPIOB, 5U);

void set_tmr3_cfg(void);
void set_tmr1_cfg(void);

int main(void)
{
  led_pin.clock_enable(true);
  led_pin.set_config(GPIO::output_push_pull);

  gen_freq.clock_enable(true);
  gen_freq.set_config(GPIO::alternate_push_pull);

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
    clock_control::pll::pll_clock_source(clock_control::PLL_CLOCK_SOURCE_Type::HSE_oscillator);
    clock_control::pll::multiplication_factor(clock_control::MULTIPLICATION_FACTOR_Type::PLL_INPUT_CLOCK_X9);
    clock_control::pll::enable(true);
    if (clock_control::pll::ready())
    {
      FLASH->ACR |= (0x02 << FLASH_ACR_LATENCY_Pos);
      clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK);
      if (clock_control::clock_switch(clock_control::SYSTEM_CLOCK_SOURCE_Type::PLL_SELECTED_AS_SYSTEM_CLOCK))
      {
        clock_control::set_ahb_prescaler(clock_control::AHB_PRESCALER_Type::SYSCLK_NOT_DIVIDED);
        clock_control::set_apb1_prescaler(clock_control::APB1_PRESCALER_Type::HCLK_DIVIDED_BY_2);
        clock_control::set_apb2_prescaler(clock_control::APB2_PRESCALER_Type::HCLK_NOT_DIVIDED);
        clock_control::set_adc_prescaler(clock_control::ADC_PRESCALER_Type::PCLK2_DIVIDED_BY_6);
      }
    }
  }

  set_tmr3_cfg();
  NVIC_EnableIRQ(TIM3_IRQn);
  set_tmr1_cfg();
  NVIC_EnableIRQ(TIM1_UP_IRQn);

  while (true)
  {
  }
}

extern "C" void TIM1_UP_IRQHandler(void)
{
  TIM1->SR = ~TIM1->SR;
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BSRR = (0b01 << 11U);
  GPIOB->BRR = (0b01 << 11U);
}

extern "C" void TIM3_IRQHandler(void)
{
  if (TIM3->SR & TIM_SR_CC2IF_Msk)
  {
    // GPIOB->BSRR = (0b01 << 11U);
    TIM1->CR1 &= ~(0b01 << TIM_CR1_CEN_Pos); // Counter enable
  }
  else
  {
    GPIOB->BRR = (0b01 << 11U);
  }
  TIM3->SR = ~TIM3->SR;
}

void set_tmr1_cfg(void)
{

  /**
   *  use apb2(72MHz) as timer clock
   */
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  TIM1->CR2 = 0x00;
  TIM1->CR2 |= (0b00 << TIM_CR2_OIS4_Pos);  // Output Idle state x (OCx output)
  TIM1->CR2 |= (0b00 << TIM_CR2_OIS3N_Pos); // Output Idle state x (OCxN output)
  TIM1->CR2 |= (0b00 << TIM_CR2_OIS3_Pos);  // Output Idle state x (OCx output)
  TIM1->CR2 |= (0b00 << TIM_CR2_OIS2N_Pos); // Output Idle state x (OCxN output)
  TIM1->CR2 |= (0b00 << TIM_CR2_OIS2_Pos);  // Output Idle state x (OCx output)
  TIM1->CR2 |= (0b00 << TIM_CR2_OIS1N_Pos); // Output Idle state x (OCxN output)
  TIM1->CR2 |= (0b00 << TIM_CR2_OIS1_Pos);  // Output Idle state x (OCx output)
  TIM1->CR2 |= (0b00 << TIM_CR2_TI1S_Pos);  // TI1 selection
  TIM1->CR2 |= (0b00 << TIM_CR2_MMS_Pos);   // !Master mode selection
  TIM1->CR2 |= (0b00 << TIM_CR2_CCDS_Pos);  // Capture/compare DMA selection
  TIM1->CR2 |= (0b00 << TIM_CR2_CCUS_Pos);  // Capture/compare control update selection
  TIM1->CR2 |= (0b00 << TIM_CR2_CCPC_Pos);  // Capture/compare preloaded control

  TIM1->SMCR = 0x00;
  TIM1->SMCR |= (0b00 << TIM_SMCR_ETP_Pos);  // External trigger polarity
  TIM1->SMCR |= (0b00 << TIM_SMCR_ECE_Pos);  // External clock enable
  TIM1->SMCR |= (0b00 << TIM_SMCR_ETPS_Pos); // External trigger prescaler
  TIM1->SMCR |= (0b00 << TIM_SMCR_ETF_Pos);  // External trigger filter
  TIM1->SMCR |= (0b00 << TIM_SMCR_MSM_Pos);  // !Master/slave mode
  TIM1->SMCR |= (0b010 << TIM_SMCR_TS_Pos);  // Trigger selection( Internal Trigger 2 (ITR2) = TIM3_TRGO)
  TIM1->SMCR |= (0b110 << TIM_SMCR_SMS_Pos); // !Slave mode selection

  TIM1->DIER = 0x00;
  TIM1->DIER |= (0b00 << TIM_DIER_TDE_Pos);   // Trigger DMA request enable
  TIM1->DIER |= (0b00 << TIM_DIER_COMDE_Pos); // COM DMA request enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC4DE_Pos); // Capture/Compare DMA request enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC3DE_Pos); // Capture/Compare DMA request enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC2DE_Pos); // Capture/Compare DMA request enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC1DE_Pos); // Capture/Compare DMA request enable
  TIM1->DIER |= (0b00 << TIM_DIER_UDE_Pos);   // Update DMA request enable
  TIM1->DIER |= (0b00 << TIM_DIER_BIE_Pos);   // Break interrupt enable
  TIM1->DIER |= (0b00 << TIM_DIER_TIE_Pos);   // Trigger interrupt enable
  TIM1->DIER |= (0b00 << TIM_DIER_COMIE_Pos); // COM interrupt enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC4IE_Pos); // Capture/Compare interrupt enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC3IE_Pos); // Capture/Compare interrupt enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC2IE_Pos); // Capture/Compare interrupt enable
  TIM1->DIER |= (0b00 << TIM_DIER_CC1IE_Pos); // Capture/Compare interrupt enable
  TIM1->DIER |= (0b01 << TIM_DIER_UIE_Pos);   // Update interrupt enable

  TIM1->EGR = 0x00;
  TIM1->EGR |= (0b00 << TIM_EGR_BG_Pos);   // Break generation
  TIM1->EGR |= (0b00 << TIM_EGR_TG_Pos);   // Trigger generation
  TIM1->EGR |= (0b00 << TIM_EGR_COMG_Pos); // Capture/Compare control update generation
  TIM1->EGR |= (0b00 << TIM_EGR_CC4G_Pos); // Capture/Compare generation
  TIM1->EGR |= (0b00 << TIM_EGR_CC3G_Pos); // Capture/Compare generation
  TIM1->EGR |= (0b00 << TIM_EGR_CC2G_Pos); // Capture/Compare generation
  TIM1->EGR |= (0b00 << TIM_EGR_CC1G_Pos); // Capture/Compare generation
  TIM1->EGR |= (0b00 << TIM_EGR_UG_Pos);   // Update generation

  TIM1->CCMR1 = 0x00;

  TIM1->CCMR2 = 0x00;

  TIM1->CCER = 0x00;

  TIM1->CNT = 0x00;

  TIM1->PSC = 72;

  TIM1->ARR = 10;

  TIM1->CR1 = 0x00;
  TIM1->CR1 |= (0b00 << TIM_CR1_CKD_Pos);  // Clock division
  TIM1->CR1 |= (0b01 << TIM_CR1_ARPE_Pos); // Auto-reload preload enable
  TIM1->CR1 |= (0b00 << TIM_CR1_CMS_Pos);  // Center-aligned mode selection
  TIM1->CR1 |= (0b00 << TIM_CR1_DIR_Pos);  // Direction
  TIM1->CR1 |= (0b00 << TIM_CR1_OPM_Pos);  // One pulse mode
  TIM1->CR1 |= (0b00 << TIM_CR1_URS_Pos);  // Update request source
  TIM1->CR1 |= (0b00 << TIM_CR1_UDIS_Pos); // Update disable
  // TIM1->CR1 |= (0b01 << TIM_CR1_CEN_Pos);  // Counter enable
}

void set_tmr3_cfg(void)
{

  /**
   *  set PB5 as TIM3_CH2
   *  use apb1(72MHz) as timer clock
   */
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP_Msk;
  AFIO->MAPR |= (0b01 << AFIO_MAPR_TIM3_REMAP_PARTIALREMAP_Pos);
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  TIM3->CCR2 = 500; // TIMx capture/compare register

  TIM3->ARR = 1000; // auto-reload register

  TIM3->PSC = 71; // Prescaler value

  TIM3->CCMR1 = 0x00;
  TIM3->CCMR1 |= (0b000 << TIM_CCMR1_CC2S_Pos);  // Capture/Compare selection(CC3 channel is configured as output)
  TIM3->CCMR1 |= (0b000 << TIM_CCMR1_OC2CE_Pos); // Output compare clear enable
  TIM3->CCMR1 |= (0b001 << TIM_CCMR1_OC2PE_Pos); // Output compare preload enable
  TIM3->CCMR1 |= (0b001 << TIM_CCMR1_OC2FE_Pos); // Output compare fast enable
  TIM3->CCMR1 |= (0b110 << TIM_CCMR1_OC2M_Pos);  // Output compare mode

  TIM3->EGR = 0x00;
  TIM3->EGR |= (0b00 << TIM_EGR_TG_Pos);   // Trigger generation
  TIM3->EGR |= (0b00 << TIM_EGR_CC4G_Pos); // Capture/compare 4 generation
  TIM3->EGR |= (0b00 << TIM_EGR_CC3G_Pos); // Capture/compare 3 generation
  TIM3->EGR |= (0b00 << TIM_EGR_CC2G_Pos); // Capture/compare 2 generation
  TIM3->EGR |= (0b00 << TIM_EGR_CC1G_Pos); // Capture/compare 1 generation
  TIM3->EGR |= (0b00 << TIM_EGR_UG_Pos);   // Update generation

  TIM3->SR = TIM3->SR;

  TIM3->DIER = 0x00;
  TIM3->DIER |= (0b00 << TIM_DIER_TDE_Pos);   // Trigger DMA request enable
  TIM3->DIER |= (0b00 << TIM_DIER_CC4DE_Pos); // Capture/Compare 4 DMA request enable
  TIM3->DIER |= (0b00 << TIM_DIER_CC3DE_Pos); // Capture/Compare 3 DMA request enable
  TIM3->DIER |= (0b00 << TIM_DIER_CC2DE_Pos); // Capture/Compare 2 DMA request enable
  TIM3->DIER |= (0b00 << TIM_DIER_CC1DE_Pos); // Capture/Compare 1 DMA request enable
  TIM3->DIER |= (0b00 << TIM_DIER_UDE_Pos);   // Update DMA request enable
  TIM3->DIER |= (0b00 << TIM_DIER_TIE_Pos);   // Trigger interrupt enable
  TIM3->DIER |= (0b00 << TIM_DIER_CC4IE_Pos); // Capture/Compare 4 interrupt enable
  TIM3->DIER |= (0b00 << TIM_DIER_CC3IE_Pos); // Capture/Compare 3 interrupt enable
  TIM3->DIER |= (0b01 << TIM_DIER_CC2IE_Pos); // Capture/Compare 2 interrupt enable
  TIM3->DIER |= (0b00 << TIM_DIER_CC1IE_Pos); // Capture/Compare 1 interrupt enable
  TIM3->DIER |= (0b01 << TIM_DIER_UIE_Pos);   // Update interrupt enable

  TIM3->SMCR = 0x00;
  TIM3->SMCR |= (0b00 << TIM_SMCR_ETP_Pos);  // External trigger polarity
  TIM3->SMCR |= (0b00 << TIM_SMCR_ECE_Pos);  // External clock enable
  TIM3->SMCR |= (0b00 << TIM_SMCR_ETPS_Pos); // External trigger prescaler
  TIM3->SMCR |= (0b00 << TIM_SMCR_ETF_Pos);  // External trigger filter
  TIM3->SMCR |= (0b00 << TIM_SMCR_MSM_Pos);  // Master/Slave mode
  TIM3->SMCR |= (0b00 << TIM_SMCR_TS_Pos);   // Trigger selection
  TIM3->SMCR |= (0b00 << TIM_SMCR_SMS_Pos);  // Slave mode selection

  TIM3->CR2 = 0x00;
  TIM3->CR2 |= (0b00 << TIM_CR2_TI1S_Pos); // настройка входа канал 1 или XOR каналов
  TIM3->CR2 |= (0b010 << TIM_CR2_MMS_Pos); // режим триггера таймера
  TIM3->CR2 |= (0b00 << TIM_CR2_CCDS_Pos); // Настройка запроса ДМА

  TIM3->CCER = 0x00;
  TIM3->CCER |= (0b00 << TIM_CCER_CC4P_Pos); //
  TIM3->CCER |= (0b00 << TIM_CCER_CC4E_Pos); //
  TIM3->CCER |= (0b00 << TIM_CCER_CC3P_Pos); //
  TIM3->CCER |= (0b00 << TIM_CCER_CC3E_Pos); //
  TIM3->CCER |= (0b00 << TIM_CCER_CC2P_Pos); // Capture/Compare output polarity
  TIM3->CCER |= (0b01 << TIM_CCER_CC2E_Pos); // Capture/Compare output enable
  TIM3->CCER |= (0b00 << TIM_CCER_CC1P_Pos); //
  TIM3->CCER |= (0b00 << TIM_CCER_CC1E_Pos); //

  TIM3->CR1 = 0x00;
  TIM3->CR1 |= (0b00 << TIM_CR1_CKD_Pos);  // коэффициент деления между тактовой частотой таймера и тактовой частотой дискретизации
  TIM3->CR1 |= (0b01 << TIM_CR1_ARPE_Pos); // Использовать теневой регистр для обновления
  TIM3->CR1 |= (0b00 << TIM_CR1_CMS_Pos);  // Центрировать сигнал
  TIM3->CR1 |= (0b00 << TIM_CR1_DIR_Pos);  // Направление счетчика
  TIM3->CR1 |= (0b00 << TIM_CR1_OPM_Pos);  // Не отключать таймер после первой итерации
  TIM3->CR1 |= (0b00 << TIM_CR1_URS_Pos);  // Источники запросов обновления
  TIM3->CR1 |= (0b00 << TIM_CR1_UDIS_Pos); // Обновление включено - для обновления теневого регистра
  TIM3->CR1 |= (0b01 << TIM_CR1_CEN_Pos);  // Запустить счетчик
}