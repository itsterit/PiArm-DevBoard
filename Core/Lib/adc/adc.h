#ifndef __ADC_H__
#define __ADC_H__
#include <stdint.h>
#include <stm32f103xb.h>
#include "adc_cr1.h"
#include "adc_cr2.h"

void adc_set_config();

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
};

#endif /* __ADC_H__ */