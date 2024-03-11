#include "main.h"

extern "C" void USART1_IRQHandler(void)
{
    USART1->SR = ~USART1->SR;
    char test = USART1->DR;
    Logger.LogD((char *)"USART1 irq\n\r");
}

void log_out_method(char *str, uint8_t len)
{
    usb_line.transmit((uint8_t *)str, len);
}