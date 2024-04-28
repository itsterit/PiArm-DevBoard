#ifndef __ADC_H__
#define __ADC_H__
#include <stdint.h>
#include <stm32f103xb.h>
#include "adc_cr1.h"
#include "adc_cr2.h"
#include "adc_smp.h"

#define ADC_END_CONVERSION(ADCx) (ADCx->SR & ADC_SR_EOS_Msk)
#define ADC_CLEAR_STATUS(ADCx) (ADCx->SR = ~(ADCx->SR))
#define ADC_START(ADCx) (ADCx->CR2 |= ADC_CR2_SWSTART_Msk)
#define ADC_DATA(ADCx) (ADCx->DR)

uint16_t get_voltage_divider_uout(uint16_t u_in_mv, uint32_t r1_high_om, uint32_t r2_low_om);
uint16_t get_voltage_divider_uin(uint16_t u_out_mv, uint32_t r1_high_om, uint32_t r2_low_om);
uint16_t get_adc_code(uint16_t refv_mv, uint16_t in_voltage_mv);
uint16_t get_adc_voltage(uint16_t refv_mv, uint16_t adc_code);
uint16_t get_adc_ref_voltage(uint16_t adc_code);

class adc
{
public:
    static bool enable(ADC_TypeDef *adc_x);

    static void set_cr1_config(ADC_TypeDef *adc_x, ADC_CR1_AWDEN_Type awden, ADC_CR1_JAWDEN_Type jawden, ADC_CR1_DUALMOD_Type dualmod,
                               uint8_t discnum, ADC_CR1_JDISCEN_Type jdiscen, ADC_CR1_DISCEN_Type discen, ADC_CR1_JAUTO_Type jauto,
                               ADC_CR1_AWDSGL_Type awdsgl, ADC_CR1_SCAN_Type scan, ADC_CR1_JEOCIE_Type jeocie, ADC_CR1_AWDIE_Type awdie,
                               ADC_CR1_EOCIE_Type eocie, uint8_t awdch);

    static void set_cr2_config(ADC_TypeDef *adc_x, ADC_CR2_TSVREFE_Type adc_cr2_tsvrefe, ADC_CR2_EXTTRIG_Type adc_cr2_exttrig, ADC_CR2_EXTSEL_Type adc_cr2_extsel,
                               ADC_CR2_JEXTTRIG_Type adc_cr2_jexttrig, ADC_CR2_JEXTSEL_Type adc_cr2_jextsel, ADC_CR2_ALIGN_Type adc_cr2_align,
                               ADC_CR2_DMA_Type adc_cr2_dma, ADC_CR2_RSTCAL_Type adc_cr2_rstcal, ADC_CR2_CONT_Type adc_cr2_cont, ADC_CR2_ADON_Type adc_cr2_adon);

    static void set_injected_sequence(ADC_TypeDef *adc_x, uint8_t injected_sequence_length, uint8_t JSQ4, uint8_t JSQ3, uint8_t JSQ2, uint8_t JSQ1);
    static bool set_regular_sequence(ADC_TypeDef *adc_x, uint8_t regular_sequence_length, uint8_t conversion_number, uint8_t conversion_channel);
    static bool set_sampling(ADC_TypeDef *adc_x, uint8_t channel, ADC_SAMPLING_CONFIG_Type sampling_cycles);
    static void set_analog_watchdog_threshold(ADC_TypeDef *adc_x, uint16_t high_threshold_code, uint16_t low_threshold_code);
};

#endif /* __ADC_H__ */