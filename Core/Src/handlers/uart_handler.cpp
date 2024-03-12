#include "main.h"

/**
 * @brief   Функция отправки если линия закончила старую транзакцию
*/
volatile void usb_as_dma_transmit(uint8_t *msg, int16_t len)
{
    if (!(DMA1_Channel4->CNDTR))
        usb_tx_dma.dma_start(len, (uint32_t *)msg, (uint32_t *)&USART1->DR);
}

/**
 * @brief   Функция конфигурирования модуля ДМА
 * @note    Применяется в случая инициализации модуля или ошибки модуля
*/
inline void set_dma_cfg()
{
    usb_tx_dma.dma_set_config(MEM2MEM_Disabled, PL_Low,
                              MSIZE_8bits, PSIZE_8bits,
                              MINC_Enabled, PINC_Disabled, CIRC_Disabled, Read_From_Memory,
                              TEIE_Enabled, HTIE_Disabled, TCIE_Enabled);
}

/**
 * @brief   Обработка прерываний передачи по ДМА как уарт1
 * @details Если дма закончила отправку жде конца передачи от уарт`а
 * @note    Обнаружив ошибку - переинициализация
*/
extern "C" void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4_Msk)
        usb_line.interrupt_config(USART_CR1_TCIE_Msk);
    else
        set_dma_cfg();
    DMA1->IFCR = (DMA_IFCR_CTEIF4_Msk | DMA_IFCR_CHTIF4_Msk | DMA_IFCR_CTCIF4_Msk | DMA_IFCR_CGIF4_Msk);
}

/**
 * @brief   Обработчик прерываний уар`а как usb
 * @details Если успешная отправка - значит это конец посылки modbus, иначе прием
*/
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

/**
 * @brief   Отправка логов
 * @note    Ждем отправку
*/
void log_out_method(char *str, uint8_t len)
{
    usb_line.transmit((uint8_t *)str, len);
}