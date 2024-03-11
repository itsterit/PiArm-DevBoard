#include "dma_control.h"

dma_control::dma_control(DMA_TypeDef *DMA_num, DMA_Channel_TypeDef *DMA_channel)
{
  this->DMA_num = DMA_num;
  this->DMA_channel = DMA_channel;
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

void dma_control::dma_set_config(eMEM2MEM_MODE memory_to_memory_mode, eCHANNEL_PRIORITY_LEVEL channel_priority_level,
                                 eMSIZE memory_size, ePSIZE peripheral_size, eMEMORY_INCREMENT_MODE memory_increment_mode,
                                 ePERIPHERAL_INCREMENT_MODE peripheral_increment_mode, eCIRCULAR_MODE circular_mode,
                                 eDATA_TRANSFER_DIRECTION data_transfer_direction,
                                 eTRANSFER_ERROR_INTERRUPT_ENABLE transfer_error_interrupt_enable,
                                 eHALF_TRANSFER_INTERRUPT_ENABLE half_transfer_interrupt_enable,
                                 eTRANSFER_COMPLETE_INTERRUPT_ENABLE transfer_complete_interrupt_enable)
{
  DMA_channel->CCR = 0x00;
  DMA_channel->CCR |=
        ((memory_to_memory_mode              & (DMA_CCR_MEM2MEM_Msk >> DMA_CCR_MEM2MEM_Pos))  << DMA_CCR_MEM2MEM_Pos) // Memory to memory mode disabled
      | ((channel_priority_level             & (DMA_CCR_PL_Msk      >> DMA_CCR_PL_Pos))       << DMA_CCR_PL_Pos)      // Channel priority level
      | ((memory_size                        & (DMA_CCR_MSIZE_Msk   >> DMA_CCR_MSIZE_Pos))    << DMA_CCR_MSIZE_Pos)   // Memory size
      | ((peripheral_size                    & (DMA_CCR_PSIZE_Msk   >> DMA_CCR_PSIZE_Pos))    << DMA_CCR_PSIZE_Pos)   // Peripheral size
      | ((memory_increment_mode              & (DMA_CCR_MINC_Msk    >> DMA_CCR_MINC_Pos))     << DMA_CCR_MINC_Pos)    // Memory increment mode
      | ((peripheral_increment_mode          & (DMA_CCR_PINC_Msk    >> DMA_CCR_PINC_Pos))     << DMA_CCR_PINC_Pos)    // Peripheral increment mode
      | ((circular_mode                      & (DMA_CCR_CIRC_Msk    >> DMA_CCR_CIRC_Pos))     << DMA_CCR_CIRC_Pos)    // Circular mode
      | ((data_transfer_direction            & (DMA_CCR_DIR_Msk     >> DMA_CCR_DIR_Pos))      << DMA_CCR_DIR_Pos)     // Data transfer direction
      | ((transfer_error_interrupt_enable    & (DMA_CCR_TEIE_Msk    >> DMA_CCR_TEIE_Pos))     << DMA_CCR_TEIE_Pos)    // Transfer error interrupt enable
      | ((half_transfer_interrupt_enable     & (DMA_CCR_HTIE_Msk    >> DMA_CCR_HTIE_Pos))     << DMA_CCR_HTIE_Pos)    // Half transfer interrupt enable
      | ((transfer_complete_interrupt_enable & (DMA_CCR_TCIE_Msk    >> DMA_CCR_TCIE_Pos))     << DMA_CCR_TCIE_Pos);   // Transfer complete interrupt enable
}

void dma_control::dma_start(uint16_t counter, uint32_t *data_pointer, uint32_t *peripheral_pointer)
{
  DMA_num->IFCR = DMA_num->IFCR;                    // сброс флагов
  DMA_channel->CMAR = (uint32_t)data_pointer;       // адрес памяти
  DMA_channel->CPAR = (uint32_t)peripheral_pointer; // адрес периферии
  DMA_channel->CNDTR = counter;                     // количество кадров для передачи
  DMA_channel->CCR |= (DMA_CCR_EN_Msk);             // Врубить канал
}