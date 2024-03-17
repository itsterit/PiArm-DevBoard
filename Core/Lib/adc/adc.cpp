#include "adc.h"

void adc_set_config()
{
    ADC2->SR = ADC2->SR;

    ADC2->CR1 = (0b00 << ADC_CR1_AWDEN_Pos)     // Analog watchdog enable on regular channels
                | (0b00 << ADC_CR1_JAWDEN_Pos)  // Analog watchdog enable on injected channels
                | (0b00 << ADC_CR1_DUALMOD_Pos) // Dual mode selection
                | (0b00 << ADC_CR1_DISCNUM_Pos) // Discontinuous mode channel count
                | (0b00 << ADC_CR1_JDISCEN_Pos) // Discontinuous mode on injected channels
                | (0b00 << ADC_CR1_DISCEN_Pos)  // Discontinuous mode on regular channels
                | (0b00 << ADC_CR1_JAUTO_Pos)   // Automatic Injected Group conversion
                | (0b00 << ADC_CR1_AWDSGL_Pos)  // Enable the watchdog on a single channel in scan mode
                | (0b00 << ADC_CR1_SCAN_Pos)    // Scan mode
                | (0b00 << ADC_CR1_JEOSIE_Pos)  // Interrupt enable for injected channels
                | (0b00 << ADC_CR1_AWDIE_Pos)   // Analog watchdog interrupt enable
                | (0b00 << ADC_CR1_EOSIE_Pos)   // Interrupt enable for EOC
                | (0b00 << ADC_CR1_AWDCH_Pos);  // Analog watchdog channel select bits

    ADC2->CR2 = (0b00 << ADC_CR2_TSVREFE_Pos)    // Temperature sensor and VREFINT enable
                | (0b00 << ADC_CR2_SWSTART_Pos)  // Start conversion of regular channels
                | (0b00 << ADC_CR2_JSWSTART_Pos) // Start conversion of injected channels
                | (0b00 << ADC_CR2_EXTTRIG_Pos)  // External trigger conversion mode for regular channels
                | (0b00 << ADC_CR2_EXTSEL_Pos)   // External event select for regular group
                | (0b00 << ADC_CR2_JEXTTRIG_Pos) // External trigger conversion mode for injected channels
                | (0b00 << ADC_CR2_JEXTSEL_Pos)  // External event select for injected group
                | (0b00 << ADC_CR2_ALIGN_Pos)    // Data alignment
                | (0b00 << ADC_CR2_DMA_Pos)      // Direct memory access mode
                | (0b00 << ADC_CR2_RSTCAL_Pos)   // Reset calibration
                | (0b00 << ADC_CR2_CAL_Pos)      // A/D Calibration
                | (0b00 << ADC_CR2_CONT_Pos)     // Continuous conversion
                | (0b00 << ADC_CR2_ADON_Pos);    // A/D converter ON / OFF

    ADC2->SMPR1 = (0b00 << ADC_SMPR1_SMP17_Pos)    // Channel x Sample time selection
                  | (0b00 << ADC_SMPR1_SMP16_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR1_SMP15_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR1_SMP14_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR1_SMP13_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR1_SMP12_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR1_SMP11_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR1_SMP10_Pos); // Channel x Sample time selection

    ADC2->SMPR2 = (0b00 << ADC_SMPR2_SMP9_Pos)    // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP8_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP7_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP6_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP5_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP4_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP3_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP2_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP1_Pos)  // Channel x Sample time selection
                  | (0b00 << ADC_SMPR2_SMP0_Pos); // Channel x Sample time selection

    ADC2->JOFR1 = (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x
    ADC2->JOFR2 = (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x
    ADC2->JOFR3 = (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x
    ADC2->JOFR4 = (0b00 << ADC_JOFR1_JOFFSET1_Pos); // Data offset for injected channel x

    ADC2->HTR = 0x00; // Analog watchdog high threshold
    ADC2->LTR = 0x00; // Analog watchdog low threshold

    ADC2->SQR1 = (0b00 << ADC_SQR1_L_Pos)       //  Regular channel sequence length
                 | (0b00 << ADC_SQR1_SQ16_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR1_SQ15_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR1_SQ14_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR1_SQ13_Pos); // Xth conversion in regular sequence

    ADC2->SQR2 = (0b00 << ADC_SQR2_SQ12_Pos)   // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR2_SQ11_Pos) // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR2_SQ10_Pos) // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR2_SQ9_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR2_SQ8_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR2_SQ7_Pos); // Xth conversion in regular sequence

    ADC2->SQR3 = (0b00 << ADC_SQR3_SQ6_Pos)    // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR3_SQ5_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR3_SQ4_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR3_SQ3_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR3_SQ2_Pos)  // Xth conversion in regular sequence
                 | (0b00 << ADC_SQR3_SQ1_Pos); // Xth conversion in regular sequence

    ADC2->JSQR = (0b00 << ADC_JSQR_JL_Pos)      // Injected sequence length
                 | (0b00 << ADC_JSQR_JSQ4_Pos)  // X conversion in injected sequence
                 | (0b00 << ADC_JSQR_JSQ3_Pos)  // X conversion in injected sequence
                 | (0b00 << ADC_JSQR_JSQ2_Pos)  // X conversion in injected sequence
                 | (0b00 << ADC_JSQR_JSQ1_Pos); // X conversion in injected sequence
}