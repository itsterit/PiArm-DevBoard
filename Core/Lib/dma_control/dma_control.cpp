#include "dma_control.h"

dma_control::dma_control(DMA_TypeDef *DMA_num, DMA_Channel_TypeDef *DMA_channel)
{
  this->DMA_num = DMA_num;
  this->DMA_channel = DMA_channel;
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

void dma_control::dma_set_config()
{
  DMA_channel->CCR = 0x00;
  DMA_channel->CCR |=
        ((0b00 & (DMA_CCR_MEM2MEM_Msk >> DMA_CCR_MEM2MEM_Pos))  << DMA_CCR_MEM2MEM_Pos) // Memory to memory mode disabled
      | ((0b00 & (DMA_CCR_PL_Msk      >> DMA_CCR_PL_Pos))       << DMA_CCR_PL_Pos)      // Channel priority level
      | ((0b00 & (DMA_CCR_MSIZE_Msk   >> DMA_CCR_MSIZE_Pos))    << DMA_CCR_MSIZE_Pos)   // Memory size
      | ((0b00 & (DMA_CCR_PSIZE_Msk   >> DMA_CCR_PSIZE_Pos))    << DMA_CCR_PSIZE_Pos)   // Peripheral size
      | ((0b00 & (DMA_CCR_MINC_Msk    >> DMA_CCR_MINC_Pos))     << DMA_CCR_MINC_Pos)    // Memory increment mode
      | ((0b00 & (DMA_CCR_PINC_Msk    >> DMA_CCR_PINC_Pos))     << DMA_CCR_PINC_Pos)    // Peripheral increment mode
      | ((0b00 & (DMA_CCR_CIRC_Msk    >> DMA_CCR_CIRC_Pos))     << DMA_CCR_CIRC_Pos)    // Circular mode
      | ((0b00 & (DMA_CCR_DIR_Msk     >> DMA_CCR_DIR_Pos))      << DMA_CCR_DIR_Pos)     // Data transfer direction
      | ((0b00 & (DMA_CCR_TEIE_Msk    >> DMA_CCR_TEIE_Pos))     << DMA_CCR_TEIE_Pos)    // Transfer error interrupt enable
      | ((0b00 & (DMA_CCR_HTIE_Msk    >> DMA_CCR_HTIE_Pos))     << DMA_CCR_HTIE_Pos)    // Half transfer interrupt enable
      | ((0b00 & (DMA_CCR_TCIE_Msk    >> DMA_CCR_TCIE_Pos))     << DMA_CCR_TCIE_Pos);   // Transfer complete interrupt enable
}

void dma_control::dma_start(uint16_t counter, uint32_t *data_pointer, uint32_t *peripheral_pointer)
{
  DMA_num->IFCR = DMA_num->IFCR;                    // сброс флагов
  DMA_channel->CNDTR = counter;                     // количество кадров для передачи
  DMA_channel->CMAR = (uint32_t)data_pointer;       // адрес памяти
  DMA_channel->CPAR = (uint32_t)peripheral_pointer; // адрес периферии
  DMA_channel->CCR |= (DMA_CCR_EN_Msk);             // Врубить канал
}