#include "main.h"

volatile void usb_as_dma_transmit(uint8_t *msg, uint16_t len)
{
}

extern "C" void DMA1_Channel4_IRQHandler(void)
{
    DMA1->IFCR = (DMA_IFCR_CTEIF4_Msk | DMA_IFCR_CHTIF4_Msk | DMA_IFCR_CTCIF4_Msk | DMA_IFCR_CGIF4_Msk);
    usb_line.interrupt_config(USART_CR1_TCIE_Msk);
}

extern "C" void USART1_IRQHandler(void)
{
    uint8_t asd = USART1->SR;
    char test = USART1->DR;
    usb_line.interrupt_config(USART_CR1_IDLEIE_Msk);
    Logger.LogD((char *)"USART1 irq\n\r");
}

void log_out_method(char *str, uint8_t len)
{
    usb_line.transmit((uint8_t *)str, len);
}