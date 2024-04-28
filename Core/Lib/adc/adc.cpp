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

    adc_x->CR2 |= (ADC_CR2_ADON_Msk);
    for (uint8_t adc_start_wait = 0; adc_start_wait < 0xFF; adc_start_wait++)
        asm("NOP");

    adc_x->SQR1 = 0x00;
    adc_x->SQR2 = 0x00;

    adc_x->CR2 |= ADC_CR2_CAL;
    for (uint8_t adc_start_wait = 0; adc_start_wait < 0xFF; adc_start_wait++)
        if ((adc_x->CR2 & ADC_CR2_CAL) == 0)
            return 1;
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
 * @brief   Установка последовательности и длины встраиваемого преобразования
 *
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

/**
 * @brief   Установка последовательности преобразований регулярных каналов
 * @details Передать номер ацп, количество преобразований(0 - одно преобразование)
 *          передать номер в последовательности преобразований(начинается с 1) и канал АЦП
 */
bool adc::set_regular_sequence(ADC_TypeDef *adc_x, uint8_t regular_sequence_length, uint8_t conversion_number, uint8_t conversion_channel)
{
    if ((conversion_number) && !(conversion_channel & ~(ADC_SQR1_SQ16_Msk >> ADC_SQR1_SQ16_Pos)) && !(regular_sequence_length & ~(ADC_SQR1_L_Msk >> ADC_SQR1_L_Pos)))
    {
        uint8_t configuration_offset = (ADC_SQR1_SQ16_Pos) - (ADC_SQR1_SQ15_Pos);

        if ((conversion_number <= 16) && (conversion_number >= 13))
        {
            conversion_number -= 13;
            adc_x->SQR1 &= ~(0x1F << (configuration_offset * conversion_number));
            adc_x->SQR1 |= (conversion_channel << (configuration_offset * conversion_number));
        }
        else if ((conversion_number <= 12) && (conversion_number >= 7))
        {
            conversion_number -= 7;
            adc_x->SQR2 &= ~(0x1F << (configuration_offset * conversion_number));
            adc_x->SQR2 |= (conversion_channel << (configuration_offset * conversion_number));
        }
        else if ((conversion_number <= 6) && (conversion_number >= 1))
        {
            conversion_number -= 1;
            adc_x->SQR3 &= ~(0x1F << (configuration_offset * conversion_number));
            adc_x->SQR3 |= (conversion_channel << (configuration_offset * conversion_number));
        }
        else
        {
            return false;
        }

        adc_x->SQR1 &= ~(ADC_SQR1_L_Msk);
        adc_x->SQR1 |= (regular_sequence_length << ADC_SQR1_L_Pos);
        return true;
    }

    return false;
}

bool adc::set_sampling(ADC_TypeDef *adc_x, uint8_t channel, ADC_SAMPLING_CONFIG_Type sampling_cycles)
{
    if (!(sampling_cycles & ~(ADC_SMPR1_SMP17_Msk >> ADC_SMPR1_SMP17_Pos)))
    {
        uint8_t configuration_offset = (ADC_SMPR1_SMP17_Pos) - (ADC_SMPR1_SMP16_Pos);

        if ((channel <= 17) && (channel >= 10))
        {
            channel -= 10;
            adc_x->SMPR1 &= ~(0b111 << (channel * configuration_offset));
            adc_x->SMPR1 |= (sampling_cycles << (channel * configuration_offset));
        }
        else if ((channel <= 9) && (channel >= 0))
        {
            adc_x->SMPR2 &= ~(0b111 << (channel * configuration_offset));
            adc_x->SMPR2 |= (sampling_cycles << (channel * configuration_offset));
        }
        else
        {
            return false;
        }
        return true;
    }
    return false;
}

void adc::set_analog_watchdog_threshold(ADC_TypeDef *adc_x, uint16_t high_threshold_code, uint16_t low_threshold_code)
{
    adc_x->HTR = high_threshold_code; // Analog watchdog high threshold
    adc_x->LTR = low_threshold_code;  // Analog watchdog low threshold
}

uint16_t get_adc_code(uint16_t refv_mv, uint16_t in_voltage_mv)
{
    if ((refv_mv < 3500) && (in_voltage_mv < 3500))
    {
        uint16_t ret_code = (4096 * in_voltage_mv) / refv_mv;
        if (!(ret_code & ~0xFFF))
        {
            return ret_code;
        }
    }
    return 0;
}

uint16_t get_voltage_divider_uin(uint16_t u_out_mv, uint32_t r1_high_om, uint32_t r2_low_om)
{
    return (uint16_t)(((u_out_mv * r1_high_om) / (r2_low_om + 1)) + u_out_mv);
}

uint16_t get_voltage_divider_uout(uint16_t u_in_mv, uint32_t r1_high_om, uint32_t r2_low_om)
{
    return (uint16_t)((float)(u_in_mv / (float)(r1_high_om + r2_low_om + 1)) * r2_low_om);
}

uint16_t get_adc_voltage(uint16_t refv_mv, uint16_t adc_code)
{
    return (uint16_t)(adc_code * (float)((float)refv_mv / 4096));
}

uint16_t get_adc_ref_voltage(uint16_t adc_code)
{
    return (uint16_t)(4915200 / ADC1->JDR1);
}

uint16_t smooth_value(float alpha, uint16_t value, uint16_t prev_value)
{
    return (alpha * value) + ((1 - alpha) * prev_value);
}