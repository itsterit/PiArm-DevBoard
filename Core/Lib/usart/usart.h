#ifndef __USART_H__
#define __USART_H__
#include <stm32f103xb.h>

/* usart_config */
typedef enum
{
    NUMBER_OF_DATA_BITS_IS_8 = 0b00,
    NUMBER_OF_DATA_BITS_IS_9 = 0b01,
} USART_WORD_LENGTH_Type;

typedef enum
{
    PARITY_CONTROL_DISABLED = 0b00,
    PARITY_CONTROL_ENABLED_EVEN = 0b010,
    PARITY_CONTROL_ENABLED_ODD = 0b011,
} PARITY_CONTROL_Type;

typedef enum
{
    NUMBER_OF_STOP_BIT_IS_1 = 0b00,
    NUMBER_OF_STOP_BIT_IS_05 = 0b01,
    NUMBER_OF_STOP_BITS_IS_2 = 0b10,
    NUMBER_OF_STOP_BIT_IS_15 = 0b11,
} STOP_BITS_Type;

class usart
{
private:
    USART_TypeDef *usart_x;

public:
    usart(USART_TypeDef *usart_x);
    bool usart_config(USART_WORD_LENGTH_Type word_length,
                      PARITY_CONTROL_Type parity_control,
                      STOP_BITS_Type stop_bits,
                      uint32_t usart_bus_clk, uint32_t baud_rate);
};

#endif /* __USART_H__ */