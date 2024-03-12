#include "main.h"

volatile void usb_as_dma_transmit(uint8_t *msg, int16_t len)
{
    if (!(DMA1_Channel4->CNDTR))
        usb_tx_dma.dma_start(len, (uint32_t *)msg, (uint32_t *)&USART1->DR);
}

inline void set_dma_cfg()
{
    usb_tx_dma.dma_set_config(MEM2MEM_Disabled, PL_Low,
                              MSIZE_8bits, PSIZE_8bits,
                              MINC_Enabled, PINC_Disabled, CIRC_Disabled, Read_From_Memory,
                              TEIE_Enabled, HTIE_Disabled, TCIE_Enabled);
}

extern "C" void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4_Msk)
        usb_line.interrupt_config(USART_CR1_TCIE_Msk);
    else
        set_dma_cfg();
    DMA1->IFCR = (DMA_IFCR_CTEIF4_Msk | DMA_IFCR_CHTIF4_Msk | DMA_IFCR_CTCIF4_Msk | DMA_IFCR_CGIF4_Msk);
}

extern "C" void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_TC_Msk)
    {
        set_dma_cfg();
    }
    USART1->SR = USART1->SR;
    char test = USART1->DR;
    usb_line.interrupt_config(USART_CR1_IDLEIE_Msk);
    // Logger.LogD((char *)"USART1 irq\n\r");
}

void log_out_method(char *str, uint8_t len)
{
    usb_line.transmit((uint8_t *)str, len);
}