#ifndef __SYSTEM_HANDLER_H__
#define __SYSTEM_HANDLER_H__
#include <main.h>
#define INVERT_GENERATOR_SIGNAL (1)

extern uint16_t act_coil_current;
void check_system_parameters();
void system_monitor();

#if INVERT_GENERATOR_SIGNAL
#define STOP_GENERATION                     \
    {                                       \
        GPIOB->CRL &= ~(GPIO_CRL_CNF5_Msk); \
        GPIOB->CRL |= (GPIO_CRL_MODE5_Msk); \
        GPIOB->BSRR = (GPIO_BSRR_BS5_Msk);  \
    }
#else
#define STOP_GENERATION                     \
    {                                       \
        GPIOB->CRL &= ~(GPIO_CRL_CNF5_Msk); \
        GPIOB->CRL |= (GPIO_CRL_MODE5_Msk); \
        GPIOB->BRR = (GPIO_BRR_BR5_Msk);    \
    }
#endif

// Частота ШИМ катушки
#define HOLDING_COIL_FREQUENCY 2
#define COIL_FREQUENCY__MAX 500
#define BASE_COIL_FREQUENCY 400
#define COIL_FREQUENCY__MIN 100
// ШИМ заполнение катушки
#define HOLDING_COIL_DUTY 1
#define COIL_DUTY__MAX 10
#define BASE_COIL_DUTY 2
#define COIL_DUTY__MIN 1
// Громкость динамика
#define HOLDING_VOLUME 3
#define VOLUME__MAX 90
#define BASE_VOLUME 50
#define VOLUME__MIN 10

// Выходные регистры
#define INPUT_REG_REF_VOLTAGE       0
#define INPUT_REG_BAT_VOLTAGE       1
#define INPUT_REG_DC_VOLTAGE        2
#define INPUT_REG_COIL_CURRENT      3
#define HOLDING_REGISTER_DATA_CRC   9

#endif /* __SYSTEM_HANDLER_H__ */