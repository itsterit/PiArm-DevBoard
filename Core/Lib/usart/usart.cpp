#include "usart.h"

usart::usart(USART_TypeDef *usart_x)
{
    this->usart_x = usart_x;

    if (usart_x == USART1)
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    else if (usart_x == USART2)
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    else if (usart_x == USART3)
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    else
        this->usart_x = 0;
}

bool usart::usart_config(USART_WORD_LENGTH_Type word_length,
                         PARITY_CONTROL_Type parity_control,
                         STOP_BITS_Type stop_bits,
                         DMA_CONFIG_Type dma_config,
                         uint32_t usart_bus_clk, uint32_t baud_rate)
{
    if ((usart_x) && (usart_bus_clk / baud_rate))
    {
        usart_x->BRR = usart_bus_clk / baud_rate;
        usart_x->SR &= ~USART1->SR;

        usart_x->CR2 = 0;
        usart_x->CR2 |= (stop_bits << USART_CR2_STOP_Pos);

        usart_x->CR3 = 0;
        usart_x->CR3 |= ((dma_config & 0b01) << USART_CR3_DMAT_Pos);
        usart_x->CR3 |= (((dma_config >> 1)) << USART_CR3_DMAR_Pos);

        usart_x->CR1 = 0;
        usart_x->CR1 |= (word_length << USART_CR1_M_Pos);
        usart_x->CR1 |= ((parity_control >> 1) << USART_CR1_PCE_Pos);
        usart_x->CR1 |= ((parity_control & 0b01) << USART_CR1_PS_Pos);
        usart_x->CR1 |= USART_CR1_TE_Msk;
        usart_x->CR1 |= USART_CR1_RE_Msk;
        usart_x->CR1 |= USART_CR1_UE_Msk;

        return 1;
    }
    return 0;
}

bool usart::transmit(uint8_t *msg, int32_t len)
{
    if (usart_x)
    {
        uint32_t transmit_err = 0;
        usart_x->SR &= ~usart_x->SR;
        usart_x->DR = (uint8_t)*msg++;

        while (transmit_err++ < 0xFFFFFFFF)
        {
            if (usart_x->SR & USART_SR_TC_Msk)
            {
                usart_x->SR &= ~usart_x->SR;
                transmit_err = 0;
                if (--len > 0)
                {
                    usart_x->DR = (uint8_t)*msg++;
                }
                else
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

bool usart::interrupt_config(uint16_t interrupt_config_msk)
{
    if (usart_x)
    {
        usart_x->CR1 &= ~(USART_CR1_TXEIE_Msk | USART_CR1_TCIE_Msk | USART_CR1_RXNEIE_Msk | USART_CR1_IDLEIE_Msk);
        usart_x->CR1 |= ((USART_CR1_TXEIE_Msk | USART_CR1_TCIE_Msk | USART_CR1_RXNEIE_Msk | USART_CR1_IDLEIE_Msk) & interrupt_config_msk);
        return 1;
    }
    return 0;
}