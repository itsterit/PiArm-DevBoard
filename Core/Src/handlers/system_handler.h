#ifndef __SYSTEM_HANDLER_H__
#define __SYSTEM_HANDLER_H__
#include <main.h>
#define INVERT_GENERATOR_SIGNAL (1)

extern uint16_t act_coil_current;
void check_system_parameters();
void system_monitor();

struct signal
{
    uint8_t signal_point_amt;
    uint16_t signal[4];
};
extern signal main_signal, search_signal;

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

// ШИМ заполнение катушки
#define HOLDING_COIL_DUTY           1
#define COIL_DUTY__MAX              10
#define BASE_COIL_DUTY              2
#define COIL_DUTY__MIN              1
// Частота ШИМ катушки
#define HOLDING_COIL_FREQUENCY      2
#define COIL_FREQUENCY__MAX         500
#define BASE_COIL_FREQUENCY         300
#define COIL_FREQUENCY__MIN         100
// Громкость динамика
#define HOLDING_VOLUME              3
#define VOLUME__MAX                 70
#define BASE_VOLUME                 10
#define VOLUME__MIN                 1
// Чувствительность
#define HOLDING_SENSITIVITY         4
#define SENSITIVITY__MAX            100
#define BASE_SENSITIVITY            10
#define SENSITIVITY__MIN            5

// регистры
#define INPUT_REG_REF_VOLTAGE       0
#define INPUT_REG_BAT_VOLTAGE       1
#define INPUT_REG_DC_VOLTAGE        2
#define INPUT_REG_COIL_CURRENT      3
#define INPUT_SEARCH_VALUE          4
#define HOLDING_REGISTER_DATA_CRC   9

#endif /* __SYSTEM_HANDLER_H__ */