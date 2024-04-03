#include "adc.h"
#include "main.h"

/**
 * @brief   Запуск и калибровка АЦП
 */
bool adc::enable(ADC_TypeDef *adc_x)
{
    uint32_t adc_enable_shift = RCC_APB2ENR_ADC2EN_Pos -
                                ((((uint32_t)(ADC2_BASE)) - (uint32_t)(adc_x)) /
                                 (((uint32_t)(ADC2_BASE)) - ((uint32_t)(ADC1_BASE))));
    RCC->APB2ENR |= (0b01 << adc_enable_shift);

    // adc_x->CR2 |= (ADC_CR2_ADON_Msk);
    // for (uint8_t adc_start_wait = 0; adc_start_wait < 0xFF; adc_start_wait++)
    //     asm("NOP");

    // adc_x->CR2 |= ADC_CR2_CAL;
    // for (uint8_t adc_start_wait = 0; adc_start_wait < 0xFF; adc_start_wait++)
    //     if ((adc_x->CR2 & ADC_CR2_CAL) == 0)
    //         return 1;
    return 0;
}

/**
 * @brief   конфигурирование control register 1
 */
void adc::set_cr1_config(ADC_TypeDef *adc_x,
                         ADC_CR1_AWDEN_Type awden,
                         ADC_CR1_JAWDEN_Type jawden,
                         ADC_CR1_DUALMOD_Type dualmod,
                         uint8_t discnum,
                         ADC_CR1_JDISCEN_Type jdiscen,
                         ADC_CR1_DISCEN_Type discen,
                         ADC_CR1_JAUTO_Type jauto,
                         ADC_CR1_AWDSGL_Type awdsgl,
                         ADC_CR1_SCAN_Type scan,
                         ADC_CR1_JEOCIE_Type jeocie,
                         ADC_CR1_AWDIE_Type awdie,
                         ADC_CR1_EOCIE_Type eocie,
                         uint8_t awdch)
{
    adc_x->CR1 = 0;
    adc_x->CR1 |= (awden << ADC_CR1_AWDEN_Pos);                             // Analog watchdog enable on regular channels
    adc_x->CR1 |= (jawden << ADC_CR1_JAWDEN_Pos);                           // Analog watchdog enable on injected channels
    adc_x->CR1 |= (dualmod << ADC_CR1_DUALMOD_Pos);                         // Dual mode selection
    adc_x->CR1 |= ((discnum << ADC_CR1_DISCNUM_Pos) & ADC_CR1_DISCNUM_Msk); // Discontinuous mode channel count
    adc_x->CR1 |= (jdiscen << ADC_CR1_JDISCEN_Pos);                         // Discontinuous mode on injected channels
    adc_x->CR1 |= (discen << ADC_CR1_DISCEN_Pos);                           // Discontinuous mode on regular channels
    adc_x->CR1 |= (jauto << ADC_CR1_JAUTO_Pos);                             // Automatic Injected Group conversion
    adc_x->CR1 |= (awdsgl << ADC_CR1_AWDSGL_Pos);                           // Enable the watchdog on a single channel in scan mode
    adc_x->CR1 |= (scan << ADC_CR1_SCAN_Pos);                               // Scan mode
    adc_x->CR1 |= (jeocie << ADC_CR1_JEOSIE_Pos);                           // Interrupt enable for injected channels
    adc_x->CR1 |= (awdie << ADC_CR1_AWDIE_Pos);                             // Analog watchdog interrupt enable
    adc_x->CR1 |= (eocie << ADC_CR1_EOSIE_Pos);                             // Interrupt enable for EOC
    adc_x->CR1 |= ((awdch << ADC_CR1_AWDCH_Pos) & ADC_CR1_AWDCH_Msk);       // Analog watchdog channel select bits
}

/**
 * @brief   конфигурирование control register 2
 * @note    программный запуск вынесен отдельной функцией
 */
void adc::set_cr2_config(ADC_TypeDef *adc_x,
                         ADC_CR2_TSVREFE_Type adc_cr2_tsvrefe,
                         ADC_CR2_EXTTRIG_Type adc_cr2_exttrig,
                         ADC_CR2_EXTSEL_Type adc_cr2_extsel,
                         ADC_CR2_JEXTTRIG_Type adc_cr2_jexttrig,
                         ADC_CR2_JEXTSEL_Type adc_cr2_jextsel,
                         ADC_CR2_ALIGN_Type adc_cr2_align,
                         ADC_CR2_DMA_Type adc_cr2_dma,
                         ADC_CR2_RSTCAL_Type adc_cr2_rstcal,
                         ADC_CR2_CONT_Type adc_cr2_cont,
                         ADC_CR2_ADON_Type adc_cr2_adon)
{
    ADC1->SQR3 = 2; // 1 преобразование - канал 0

    adc_x->CR2 = 0;
    adc_x->CR2 |= (adc_cr2_tsvrefe << ADC_CR2_TSVREFE_Pos);   // Temperature sensor and VREFINT enable
    adc_x->CR2 |= (0b00 << ADC_CR2_SWSTART_Pos);              // Start conversion of regular channels
    adc_x->CR2 |= (0b00 << ADC_CR2_JSWSTART_Pos);             // Start conversion of injected channels
    adc_x->CR2 |= (adc_cr2_exttrig << ADC_CR2_EXTTRIG_Pos);   // External trigger conversion mode for regular channels
    adc_x->CR2 |= (adc_cr2_extsel << ADC_CR2_EXTSEL_Pos);     // External event select for regular group
    adc_x->CR2 |= (adc_cr2_jexttrig << ADC_CR2_JEXTTRIG_Pos); // External trigger conversion mode for injected channels
    adc_x->CR2 |= (adc_cr2_jextsel << ADC_CR2_JEXTSEL_Pos);   // External event select for injected group
    adc_x->CR2 |= (adc_cr2_align << ADC_CR2_ALIGN_Pos);       // Data alignment
    adc_x->CR2 |= (adc_cr2_dma << ADC_CR2_DMA_Pos);           // Direct memory access mode
    adc_x->CR2 |= (adc_cr2_rstcal << ADC_CR2_RSTCAL_Pos);     // Reset calibration
    adc_x->CR2 |= (adc_cr2_cont << ADC_CR2_CONT_Pos);         // Continuous conversion
    adc_x->CR2 |= (adc_cr2_adon << ADC_CR2_ADON_Pos);         // A/D converter ON / OFF
}

/**
 * @brief
 * 
 * @details
 * @warning В отличие от обычной последовательности преобразования, если длина JL[1:0] меньше четырех, каналы
 *          преобразуются в последовательности, начинающейся с (4-JL). Пример: ADC_J SQR[21:0] = 10
 *          00011 00011 00111 00010 означает, что при преобразовании сканирования будет преобразована следующая
 *          последовательность каналов: 7, 3, 3. (не 2, 7, 3)
 *          Биты 14:10 S34:01: третье преобразование в введенной последовательности (когда l1:01 =3)
*/
void adc::set_injected_sequence(ADC_TypeDef *adc_x, uint8_t injected_sequence_length, uint8_t JSQ4, uint8_t JSQ3, uint8_t JSQ2, uint8_t JSQ1)
{
    adc_x->JSQR = 0x00;
    adc_x->JSQR |= ((injected_sequence_length << ADC_JSQR_JL_Pos) & ADC_JSQR_JL_Msk);
    adc_x->JSQR |= ((JSQ4 << ADC_JSQR_JSQ4_Pos) & ADC_JSQR_JSQ4_Msk);
    adc_x->JSQR |= ((JSQ3 << ADC_JSQR_JSQ3_Pos) & ADC_JSQR_JSQ3_Msk);
    adc_x->JSQR |= ((JSQ2 << ADC_JSQR_JSQ2_Pos) & ADC_JSQR_JSQ2_Msk);
    adc_x->JSQR |= ((JSQ1 << ADC_JSQR_JSQ1_Pos) & ADC_JSQR_JSQ1_Msk);
}

void adc_set_config()
{

    // ADC1->SMPR1 |= (0b00 << ADC_SMPR1_SMP17_Pos)    // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR1_SMP16_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR1_SMP15_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR1_SMP14_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR1_SMP13_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR1_SMP12_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR1_SMP11_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR1_SMP10_Pos); // Channel x Sample time selection

    // ADC1->SMPR2 |= (0b00 << ADC_SMPR2_SMP9_Pos)    // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP8_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP7_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP6_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP5_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP4_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP3_Pos)  // Channel x Sample time selection
    //                | (0b01 << ADC_SMPR2_SMP2_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP1_Pos)  // Channel x Sample time selection
    //                | (0b00 << ADC_SMPR2_SMP0_Pos); // Channel x Sample time selection

    // ADC2->CR2 |= (0b00 << ADC_CR2_TSVREFE_Pos)    // Temperature sensor and VREFINT enable
    //              | (0b00 << ADC_CR2_SWSTART_Pos)  // Start conversion of regular channels
    //              | (0b00 << ADC_CR2_JSWSTART_Pos) // Start conversion of injected channels
    //              | (0b00 << ADC_CR2_EXTTRIG_Pos)  // External trigger conversion mode for regular channels
    //              | (0b00 << ADC_CR2_EXTSEL_Pos)   // External event select for regular group
    //              | (0b00 << ADC_CR2_JEXTTRIG_Pos) // External trigger conversion mode for injected channels
    //              | (0b00 << ADC_CR2_JEXTSEL_Pos)  // External event select for injected group
    //              | (0b00 << ADC_CR2_ALIGN_Pos)    // Data alignment
    //              | (0b00 << ADC_CR2_DMA_Pos)      // Direct memory access mode
    //              | (0b00 << ADC_CR2_RSTCAL_Pos)   // Reset calibration
    //              | (0b00 << ADC_CR2_CAL_Pos)      // A/D Calibration
    //              | (0b00 << ADC_CR2_CONT_Pos)     // Continuous conversion
    //              | (0b00 << ADC_CR2_ADON_Pos);    // A/D converter ON / OFF

    // ADC2->JOFR1 |= (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x
    // ADC2->JOFR2 |= (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x
    // ADC2->JOFR3 |= (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x
    // ADC2->JOFR4 |= (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x

    // ADC2->HTR = 0x00; // Analog watchdog high threshold
    // ADC2->LTR = 0x00; // Analog watchdog low threshold

    // ADC2->SQR1 |= (0b00 << ADC_SQR1_L_Pos)       //  Regular channel sequence length
    //               | (0b00 << ADC_SQR1_SQ16_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR1_SQ15_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR1_SQ14_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR1_SQ13_Pos); // Xth conversion in regular sequence

    // ADC2->SQR2 |= (0b00 << ADC_SQR2_SQ12_Pos)   // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR2_SQ11_Pos) // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR2_SQ10_Pos) // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR2_SQ9_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR2_SQ8_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR2_SQ7_Pos); // Xth conversion in regular sequence

    // ADC2->SQR3 |= (0b00 << ADC_SQR3_SQ6_Pos)    // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR3_SQ5_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR3_SQ4_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR3_SQ3_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR3_SQ2_Pos)  // Xth conversion in regular sequence
    //               | (0b00 << ADC_SQR3_SQ1_Pos); // Xth conversion in regular sequence

    // ADC2->JSQR |= (0b00 << ADC_JSQR_JL_Pos)      // Injected sequence length
    //               | (0b00 << ADC_JSQR_JSQ4_Pos)  // X conversion in injected sequence
    //               | (0b00 << ADC_JSQR_JSQ3_Pos)  // X conversion in injected sequence
    //               | (0b00 << ADC_JSQR_JSQ2_Pos)  // X conversion in injected sequence
    //               | (0b00 << ADC_JSQR_JSQ1_Pos); // X conversion in injected sequence
}