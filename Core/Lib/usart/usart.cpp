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
                         uint32_t usart_bus_clk, uint32_t baud_rate)
{
    if ((usart_x) && (usart_bus_clk / baud_rate))
    {
        USART1->BRR = usart_bus_clk / baud_rate;
        USART1->SR &= ~USART1->SR;

        USART1->CR2 &= ~USART_CR2_STOP_Msk;
        USART1->CR2 |= (stop_bits << USART_CR2_STOP_Pos);

        USART1->CR1 &= ~USART_CR1_M_Msk;
        USART1->CR1 |= (word_length << USART_CR1_M_Pos);

        USART1->CR1 &= ~USART_CR1_PCE_Msk;
        USART1->CR1 |= ((parity_control >> 1) << USART_CR1_PCE_Pos);

        USART1->CR1 &= ~USART_CR1_PS_Msk;
        USART1->CR1 |= ((parity_control & 0b01) << USART_CR1_PS_Pos);

        USART1->CR1 |= USART_CR1_TE_Msk;
        USART1->CR1 |= USART_CR1_RE_Msk;
        USART1->CR1 |= USART_CR1_UE_Msk;

        return 1;
    }
    return 0;
}