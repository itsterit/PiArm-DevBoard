#include <main.h>

#define COIL_CURRENT_ADC_CHANNEL (2)
#define VOLTAGE_CONVERTER_ADC_CHANNEL (3)
#define BATTERY_VOLTAGE_ADC_CHANNEL (4)
#define REFERENCE_VOLTAGE_ADC_CHANNEL (17)
#define REFERENCE_VOLTAGE_HIGH (3500)
#define REFERENCE_VOLTAGE_LOW (2750)
#define BAT_VOLTAGE_HIGH (6000)
#define BAT_VOLTAGE_LOW (3200)
#define DC_VOLTAGE_HIGH (15000)
#define DC_VOLTAGE_LOW (11000)

volatile bool system_started_flag = 0;
volatile uint16_t ref_voltage = 0;
volatile uint16_t coil_current = 0;

/**
 * @brief   Получение опорного напряжения
 * @retval  Вернем true, если получили опорное напряжение
 */
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

bool adc_start_system_monitor(uint16_t rev_mv)
{
    // включить тактирование
    if (adc::enable(ADC1))
    {
        // конфигурация
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

        // установка времени и порядка сэмплирования и настройка watchdog`а по току катушки
        if (adc::set_sampling(ADC1, VOLTAGE_CONVERTER_ADC_CHANNEL, SMP_239_5_cycles) && // Преобразователь  - 3й канал
            adc::set_sampling(ADC1, BATTERY_VOLTAGE_ADC_CHANNEL, SMP_239_5_cycles) &&   // Акб              - 4й канал
            adc::set_sampling(ADC1, REFERENCE_VOLTAGE_ADC_CHANNEL, SMP_239_5_cycles))   // Опора            - 17й канал
        {
            adc::set_injected_sequence(ADC1, 2, VOLTAGE_CONVERTER_ADC_CHANNEL, BATTERY_VOLTAGE_ADC_CHANNEL, REFERENCE_VOLTAGE_ADC_CHANNEL, 0);
            return 1;
        }
    }
    return 0;
}

get_system_status system_monitor_handler(uint16_t ref_voltage, uint16_t bat_voltage, uint16_t dc_voltage)
{
    if ((REFERENCE_VOLTAGE_LOW < ref_voltage) && (ref_voltage < REFERENCE_VOLTAGE_HIGH))
    {
        if ((BAT_VOLTAGE_LOW < bat_voltage) && (bat_voltage < BAT_VOLTAGE_HIGH))
        {
            if ((DC_VOLTAGE_LOW < dc_voltage) && (dc_voltage < DC_VOLTAGE_HIGH))
            {
                return SYSTEM_OK;
            }
            else
            {
                return DC_VOLTAGE_ERR;
            }
        }
    }
    return BAT_VOLTAGE_ERR;
}