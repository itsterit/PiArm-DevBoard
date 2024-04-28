#include <main.h>

#define COIL_CURRENT_ADC_CHANNEL (2)
#define VOLTAGE_CONVERTER_ADC_CHANNEL (3)
#define BATTERY_VOLTAGE_ADC_CHANNEL (4)
#define REFERENCE_VOLTAGE_ADC_CHANNEL (17)

#define REFERENCE_VOLTAGE_HIGH (3400)
#define REFERENCE_VOLTAGE_LOW (3200)

#define BAT_VOLTAGE_HIGH (4200)
#define BAT_VOLTAGE_LOW (5200)

#define DC_VOLTAGE_HIGH (14000)
#define DC_VOLTAGE_LOW (12000)

typedef enum
{
    OK = 0b01,
    ERR = 0b00,
} VOLTAGE_MONITOR_STATUS_Type;
struct SYSTEM_MONITOR_STATUS
{
    VOLTAGE_MONITOR_STATUS_Type reference_voltage_status = ERR;
    VOLTAGE_MONITOR_STATUS_Type bat_voltage_status = ERR;
    VOLTAGE_MONITOR_STATUS_Type dc_voltage_status = ERR;
} system_monitor_status;

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

    adc::set_sampling(ADC1, COIL_CURRENT_ADC_CHANNEL, SMP_7_5_cycles);       // Ток катушки
    adc::set_sampling(ADC1, VOLTAGE_CONVERTER_ADC_CHANNEL, SMP_7_5_cycles);  // Преобразователь
    adc::set_sampling(ADC1, BATTERY_VOLTAGE_ADC_CHANNEL, SMP_7_5_cycles);    // Акб
    adc::set_sampling(ADC1, REFERENCE_VOLTAGE_ADC_CHANNEL, SMP_71_5_cycles); // Опора

    adc::set_injected_sequence(ADC1, 2,
                               3, 4, 17, 0);

    adc::set_analog_watchdog_threshold(ADC1,
                                       get_adc_code(ref_voltage, 200), // 1.5A
                                       0);
    adc::set_regular_sequence(ADC1, 0, 1, 2);
    ADC_START(ADC1);
}

void system_monitor_handler()
{
    if (ADC1->SR & ADC_SR_JEOS_Msk)
    {
        {
            usInputRegisters[INPUT_REG_REF_VOLTAGE] = get_adc_ref_voltage(ADC1->JDR1);
            usInputRegisters[INPUT_REG_BAT_VOLTAGE] = get_voltage_divider_uin(get_adc_voltage(ref_voltage, ADC1->JDR2), 10000, 5100);
            usInputRegisters[INPUT_REG_DC_VOLTAGE] = get_voltage_divider_uin(get_adc_voltage(ref_voltage, ADC1->JDR3), 1000, 100);

            // if (REFERENCE_VOLTAGE_LOW <= ref_voltage <= REFERENCE_VOLTAGE_HIGH)
            // {
            //     system_monitor_status.reference_voltage_status = OK;
            // }
            // if (BAT_VOLTAGE_LOW <= (uint16_t)(ADC1->JDR2 * (float)((float)ref_voltage / 4096)) <= BAT_VOLTAGE_HIGH)
            // {
            //     dc_enable.set();
            //     system_monitor_status.bat_voltage_status = OK;
            // }
            // if (DC_VOLTAGE_LOW <= (uint16_t)(ADC1->JDR3 * (float)((float)ref_voltage / 4096)) <= DC_VOLTAGE_HIGH)
            // {
            //     system_monitor_status.dc_voltage_status = OK;
            // }

            // Logger.LogD((char *)"Coil_cur   (%d)\n\r", (uint16_t)(coil_current * (float)((float)ref_voltage / 4096)));
            // Logger.LogD((char *)"ref        (%d)\n\r", usInputRegisters[INPUT_REG_REF_VOLTAGE]);
            // Logger.LogD((char *)"bat        (%d)\n\r", usInputRegisters[INPUT_REG_BAT_VOLTAGE]);
            // Logger.LogD((char *)"dc         (%d)\n\n\r", usInputRegisters[INPUT_REG_DC_VOLTAGE]);
            ADC1->SR = ~ADC1->SR;
        }
        {
            if ((system_monitor_status.reference_voltage_status == OK) &&
                (system_monitor_status.bat_voltage_status == OK) &&
                (system_monitor_status.dc_voltage_status == OK))
            {
                led_pin.set();
                cur_fault_delay = 6000;
            }
        }
    }
}

/**
 * @brief   Превышение тока катушки
 *
 * @details Временно отключить генерацию
 */
extern "C" void ADC1_2_IRQHandler(void)
{
    __disable_irq();
    {
        GPIOB->CRL &= ~(GPIO_CRL_CNF5_Msk);
        GPIOB->CRL |= (GPIO_CRL_MODE5_Msk);
        GPIOB->BSRR = (GPIO_BSRR_BS5_Msk);

        while (cur_fault.get_level() == 0)
            asm("NOP");

        ADC1->SR = ~ADC1->SR;
        cur_fault_delay = 0xFF;
        led_pin.set();
    }
    __enable_irq();
}
