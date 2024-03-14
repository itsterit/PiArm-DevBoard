#include "main.h"

/**
 * @brief   Функция отправки если линия закончила старую транзакцию
 *          ибо иначе данные не перезапишутся.
 */
void usb_as_dma_transmit(uint8_t *msg, int16_t len)
{
    if (USB_BUFFER_SIZE >= len)
        usb_tx_dma.dma_start(len, (uint32_t *)msg, (uint32_t *)&USART1->DR);
}

/**
 * @brief   Функция конфигурирования модуля ДМА на передачу и прием
 * @note    Применяется в случая инициализации модуля, ошибки модуля или ошибки размера буфера
 */
void set_usb_tx_dma_cfg()
{
    usb_tx_dma.dma_set_config(MEM2MEM_Disabled, PL_Low,
                              MSIZE_8bits, PSIZE_8bits,
                              MINC_Enabled, PINC_Disabled, CIRC_Disabled, Read_From_Memory,
                              TEIE_Enabled, HTIE_Disabled, TCIE_Enabled);
}

void set_usb_rx_dma_cfg()
{
    usb_rx_dma.dma_set_config(MEM2MEM_Disabled, PL_Low,
                              MSIZE_8bits, PSIZE_8bits,
                              MINC_Enabled, PINC_Disabled, CIRC_Disabled, Read_From_Peripheral,
                              TEIE_Enabled, HTIE_Disabled, TCIE_Enabled);
    usb_rx_dma.dma_start(USB_BUFFER_SIZE, (uint32_t *)&usb_buffer[0], (uint32_t *)&USART1->DR);
}

/**
 * @brief   Обработка прерываний передачи по ДМА как уарт1
 * @details Если дма закончила отправку жде конца передачи от уарт`а
 * @note    Обнаружив ошибку - переинициализация
 */
extern "C" void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4_Msk)
        usb_line.interrupt_config(USART_CR1_TCIE_Msk | USART_CR1_IDLEIE_Msk);
    else
        set_usb_tx_dma_cfg();
    DMA1->IFCR = (DMA_IFCR_CTEIF4_Msk | DMA_IFCR_CHTIF4_Msk | DMA_IFCR_CTCIF4_Msk | DMA_IFCR_CGIF4_Msk);
}

extern "C" void DMA1_Channel5_IRQHandler(void)
{
    set_usb_rx_dma_cfg();
    USART1->SR = USART1->SR;
    DMA1->IFCR = (DMA_IFCR_CTEIF5_Msk | DMA_IFCR_CHTIF5_Msk | DMA_IFCR_CTCIF5_Msk | DMA_IFCR_CGIF5_Msk);
}

/**
 * @brief   Обработчик прерываний уар`а как usb
 * @details Если успешная отправка - значит это конец посылки modbus, иначе прием
 */
extern "C" void USART1_IRQHandler(void)
{
    usb_line.interrupt_config(USART_CR1_IDLEIE_Msk);
    if (USART1->SR & USART_SR_TC_Msk)
    {
        set_usb_tx_dma_cfg();
    }
    if (USART1->SR & USART_SR_IDLE_Msk)
    {
        ModBus.FrameHandler((uint8_t *)&usb_buffer[0], (USB_BUFFER_SIZE - DMA1_Channel5->CNDTR), MODBUS_ADDRESS, USB_BUFFER_SIZE);
        set_usb_rx_dma_cfg();
    }
    USART1->SR = USART1->SR;
    volatile char clear_uart_data_register __attribute__((unused)) = USART1->DR;
}

/**
 * @brief   Отправка логов
 * @note    Ждем отправку
 */
void log_out_method(char *str, uint8_t len)
{
    usb_line.transmit((uint8_t *)str, len);
}
