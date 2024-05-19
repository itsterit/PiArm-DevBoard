#include <main.h>
void check_system_parameters();

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
#define HOLDING_COIL_FREQUENCY      2
#define COIL_FREQUENCY__MAX         5000
#define BASE_COIL_FREQUENCY         500
#define COIL_FREQUENCY__MIN         100
// ШИМ заполнение катушки
#define HOLDING_COIL_DUTY           1
#define COIL_DUTY__MAX              20
#define BASE_COIL_DUTY              2
#define COIL_DUTY__MIN              1
// Громкость динамика
#define HOLDING_VOLUME              3
#define VOLUME__MAX                 90
#define BASE_VOLUME                 50
#define VOLUME__MIN                 10

// Выходные регистры
#define INPUT_REG_REF_VOLTAGE       0
#define INPUT_REG_BAT_VOLTAGE       1
#define INPUT_REG_DC_VOLTAGE        2
#define HOLDING_REGISTER_DATA_CRC   9