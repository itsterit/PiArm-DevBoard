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
    static void set_cr1_config(ADC_TypeDef *adc_x,
                               AWDEN_Type awden, JAWDEN_Type jawden,
                               DUALMOD_Type dualmod,
                               uint8_t discnum,
                               JDISCEN_Type jdiscen, DISCEN_Type discen,
                               JAUTO_Type jauto,
                               AWDSGL_Type awdsgl,
                               SCAN_Type scan,
                               JEOCIE_Type jeocie, AWDIE_Type awdie, EOCIE_Type eocie, uint8_t awdch);
};

#endif /* __ADC_H__ */