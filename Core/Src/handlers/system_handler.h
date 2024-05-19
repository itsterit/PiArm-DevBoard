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