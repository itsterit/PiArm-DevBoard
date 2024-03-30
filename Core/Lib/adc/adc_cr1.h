#ifndef __ADC_CR1_H__
#define __ADC_CR1_H__

typedef enum
{
    AWDEN__REGULAR_CHANNELS_ANALOG_WATCHDOG_DISABLED                      = 0b00,
    AWDEN__REGULAR_CHANNELS_ANALOG_WATCHDOG_ENABLED                       = 0b01,
} ADC_CR1_AWDEN_Type;

typedef enum
{
    JAWDEN__INJECTED_CHANNELS_ANALOG_WATCHDOG_DISABLED                    = 0b00,
    JAWDEN__INJECTED_CHANNELS_ANALOG_WATCHDOG_ENABLED                     = 0b01,
} ADC_CR1_JAWDEN_Type;

typedef enum
{
    DUALMOD__INDEPENDENT_MODE                                             = 0b0000,
    DUALMOD__COMBINED_REGULAR_SIMULTANEOUS_AND_INJECTED_SIMULTANEOUS_MODE = 0b0001,
    DUALMOD__COMBINED_REGULAR_SIMULTANEOUS_AND_ALTERNATE_TRIGGER_MODE     = 0b0010,
    DUALMOD__COMBINED_INJECTED_SIMULTANEOUS_AND_FAST_INTERLEAVED_MODE     = 0b0011,
    DUALMOD__COMBINED_INJECTED_SIMULTANEOUS_AND_SLOW_INTERLEAVED_MODE     = 0b0100,
    DUALMOD__INJECTED_SIMULTANEOUS_MODE_ONLY                              = 0b0101,
    DUALMOD__REGULAR_SIMULTANEOUS_MODE_ONLY                               = 0b0110,
    DUALMOD__FAST_INTERLEAVED_MODE_ONLY                                   = 0b0111,
    DUALMOD__SLOW_INTERLEAVED_MODE_ONLY                                   = 0b1000,
    DUALMOD__ALTERNATE_TRIGGER_MODE_ONLY                                  = 0b1001,
} ADC_CR1_DUALMOD_Type;

typedef enum
{
    JDISCEN__INJECTED_CHANNELS_DISCONTINUOUS_MODE_DISABLED                = 0b00,
    JDISCEN__INJECTED_CHANNELS_DISCONTINUOUS_MODE_ENABLED                 = 0b01,
} ADC_CR1_JDISCEN_Type;

typedef enum
{
    DISCEN__REGULAR_CHANNELS_DISCONTINUOUS_MODE_DISABLED                  = 0b00,
    DISCEN__REGULAR_CHANNELS_DISCONTINUOUS_MODE_ENABLED                   = 0b01,
} ADC_CR1_DISCEN_Type;

typedef enum
{
    JAUTO__AUTOMATIC_INJECTED_CONVERSION_DISABLED                         = 0b00,
    JAUTO__AUTOMATIC_INJECTED_CONVERSION_ENABLED                          = 0b01,
} ADC_CR1_JAUTO_Type;

typedef enum
{
    AWDSGL__ANALOG_WATCHDOG_ON_ALL_CHANNELS                               = 0b00,
    AWDSGL__ANALOG_WATCHDOG_ON_SINGLE_CHANNEL                             = 0b01,
} ADC_CR1_AWDSGL_Type;

typedef enum
{
    SCAN__SCAN_MODE_DISABLED                                              = 0b00,
    SCAN__SCAN_MODE_ENABLED                                               = 0b01,
} ADC_CR1_SCAN_Type;

typedef enum
{
    JEOCIE__JEOC_INTERRUPT_DISABLED                                       = 0b00,
    JEOCIE__JEOC_INTERRUPT_ENABLED                                        = 0b01,
} ADC_CR1_JEOCIE_Type;

typedef enum
{
    AWDIE__ANALOG_WATCHDOG_INTERRUPT_DISABLED                             = 0b00,
    AWDIE__ANALOG_WATCHDOG_INTERRUPT_ENABLED                              = 0b01,
} ADC_CR1_AWDIE_Type;

typedef enum
{
    EOCIE__EOC_INTERRUPT_DISABLED                                         = 0b00,
    EOCIE__EOC_INTERRUPT_ENABLED                                          = 0b01,
} ADC_CR1_EOCIE_Type;

#endif /* __ADC_CR1_H__ */