#include <main.h>

volatile uint16_t ref_voltage = 0;
volatile uint16_t coil_current = 0;

bool get_core_voltage(uint16_t *ret_data)
{
    adc::enable(ADC1);
    adc::set_cr1_config(ADC1, AWDEN__REGULAR_CHANNELS_ANALOG_WATCHDOG_DISABLED, JAWDEN__INJECTED_CHANNELS_ANALOG_WATCHDOG_DISABLED,
                        DUALMOD__INDEPENDENT_MODE, 0, JDISCEN__INJECTED_CHANNELS_DISCONTINUOUS_MODE_DISABLED,
                        DISCEN__REGULAR_CHANNELS_DISCONTINUOUS_MODE_DISABLED, JAUTO__AUTOMATIC_INJECTED_CONVERSION_DISABLED,
                        AWDSGL__ANALOG_WATCHDOG_ON_ALL_CHANNELS, SCAN__SCAN_MODE_DISABLED, JEOCIE__JEOC_INTERRUPT_DISABLED,
                        AWDIE__ANALOG_WATCHDOG_INTERRUPT_DISABLED, EOCIE__EOC_INTERRUPT_DISABLED, 0);
    adc::set_cr2_config(ADC1, TSVREFE__TEMPERATURE_SENSOR_VREFINT_CHANNEL_ENABLED, EXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED,
                        EXTSEL__SWSTART, JEXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_DISABLED, JEXTSEL__JSWSTART,
                        ALIGN__RIGHT_ALIGNMENT, DMA__DMA_MODE_DISABLED, RSTCAL__CALIBRATION_REGISTER_INITIALIZED,
                        CONT__SINGLE_CONVERSION_MODE, ADON__ENABLE_ADC);
    NVIC_DisableIRQ(ADC1_2_IRQn);

    ADC_CLEAR_STATUS(ADC1);
    if (adc::set_regular_sequence(ADC1, 0, 1, 17) && adc::set_sampling(ADC1, 17, SMP_239_5_cycles))
    {
        ADC_START(ADC1);
        for (uint16_t cnt = 0; cnt < 0xfff; cnt++)
        {
            if (ADC_END_CONVERSION(ADC1))
            {
                ADC_CLEAR_STATUS(ADC1);
                *ret_data = (4915200 / ADC_DATA(ADC1));
                return true;
            }
        }
    }
    return false;
}

void adc_start_system_monitor()
{
    adc::enable(ADC1);
    adc::set_cr1_config(ADC1, AWDEN__REGULAR_CHANNELS_ANALOG_WATCHDOG_ENABLED, JAWDEN__INJECTED_CHANNELS_ANALOG_WATCHDOG_DISABLED,
                        DUALMOD__INDEPENDENT_MODE, 0, JDISCEN__INJECTED_CHANNELS_DISCONTINUOUS_MODE_DISABLED,
                        DISCEN__REGULAR_CHANNELS_DISCONTINUOUS_MODE_DISABLED, JAUTO__AUTOMATIC_INJECTED_CONVERSION_DISABLED,
                        AWDSGL__ANALOG_WATCHDOG_ON_SINGLE_CHANNEL, SCAN__SCAN_MODE_ENABLED,
                        JEOCIE__JEOC_INTERRUPT_DISABLED, AWDIE__ANALOG_WATCHDOG_INTERRUPT_ENABLED, EOCIE__EOC_INTERRUPT_DISABLED, 2);

    adc::set_cr2_config(ADC1, TSVREFE__TEMPERATURE_SENSOR_VREFINT_CHANNEL_ENABLED,
                        EXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED, EXTSEL__SWSTART,
                        JEXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED, JEXTSEL__JSWSTART,
                        ALIGN__RIGHT_ALIGNMENT, DMA__DMA_MODE_DISABLED, RSTCAL__CALIBRATION_REGISTER_INITIALIZED,
                        CONT__CONTINUOUS_CONVERSION_MODE, ADON__ENABLE_ADC);

    adc::set_sampling(ADC1, 2, SMP_7_5_cycles);    // Ток катушки
    adc::set_sampling(ADC1, 3, SMP_7_5_cycles);    // Преобразователь
    adc::set_sampling(ADC1, 4, SMP_7_5_cycles);    // Акб
    adc::set_sampling(ADC1, 17, SMP_7_5_cycles); // Опора

    adc::set_injected_sequence(ADC1, 2,
                               3, 4, 17, 0);

    adc::set_analog_watchdog_threshold(ADC1,
                                       get_adc_code(ref_voltage, 200), // 1.5A
                                       0);
    adc::set_regular_sequence(ADC1, 0, 1, 2);
    ADC_START(ADC1);
}