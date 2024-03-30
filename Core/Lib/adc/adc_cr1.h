#ifndef __ADC_CR1_H__
#define __ADC_CR1_H__

typedef enum
{
    REGULAR_CHANNELS_ANALOG_WATCHDOG_DISABLED                    = 0b00,
    REGULAR_CHANNELS_ANALOG_WATCHDOG_ENABLED                     = 0b01,
} ADC_CR1_AWDEN_Type;

typedef enum
{
    INJECTED_CHANNELS_ANALOG_WATCHDOG_DISABLED                   = 0b00,
    INJECTED_CHANNELS_ANALOG_WATCHDOG_ENABLED                    = 0b01,
} ADC_CR1_JAWDEN_Type;

typedef enum
{
    INDEPENDENT_MODE                                             = 0b0000,
    COMBINED_REGULAR_SIMULTANEOUS_AND_INJECTED_SIMULTANEOUS_MODE = 0b0001,
    COMBINED_REGULAR_SIMULTANEOUS_AND_ALTERNATE_TRIGGER_MODE     = 0b0010,
    COMBINED_INJECTED_SIMULTANEOUS_AND_FAST_INTERLEAVED_MODE     = 0b0011,
    COMBINED_INJECTED_SIMULTANEOUS_AND_SLOW_INTERLEAVED_MODE     = 0b0100,
    INJECTED_SIMULTANEOUS_MODE_ONLY                              = 0b0101,
    REGULAR_SIMULTANEOUS_MODE_ONLY                               = 0b0110,
    FAST_INTERLEAVED_MODE_ONLY                                   = 0b0111,
    SLOW_INTERLEAVED_MODE_ONLY                                   = 0b1000,
    ALTERNATE_TRIGGER_MODE_ONLY                                  = 0b1001,
} ADC_CR1_DUALMOD_Type;

typedef enum
{
    INJECTED_CHANNELS_DISCONTINUOUS_MODE_DISABLED                = 0b00,
    INJECTED_CHANNELS_DISCONTINUOUS_MODE_ENABLED                 = 0b01,
} ADC_CR1_JDISCEN_Type;

typedef enum
{
    REGULAR_CHANNELS_DISCONTINUOUS_MODE_DISABLED                 = 0b00,
    REGULAR_CHANNELS_DISCONTINUOUS_MODE_ENABLED                  = 0b01,
} ADC_CR1_DISCEN_Type;

typedef enum
{
    AUTOMATIC_INJECTED_CONVERSION_DISABLED                       = 0b00,
    AUTOMATIC_INJECTED_CONVERSION_ENABLED                        = 0b01,
} ADC_CR1_JAUTO_Type;

typedef enum
{
    ANALOG_WATCHDOG_ON_ALL_CHANNELS                              = 0b00,
    ANALOG_WATCHDOG_ON_SINGLE_CHANNEL                            = 0b01,
} ADC_CR1_AWDSGL_Type;

typedef enum
{
    SCAN_MODE_DISABLED                                           = 0b00,
    SCAN_MODE_ENABLED                                            = 0b01,
} ADC_CR1_SCAN_Type;

typedef enum
{
    JEOC_INTERRUPT_DISABLED                                      = 0b00,
    JEOC_INTERRUPT_ENABLED                                       = 0b01,
} ADC_CR1_JEOCIE_Type;

typedef enum
{
    ANALOG_WATCHDOG_INTERRUPT_DISABLED                           = 0b00,
    ANALOG_WATCHDOG_INTERRUPT_ENABLED                            = 0b01,
} ADC_CR1_AWDIE_Type;

typedef enum
{
    EOC_INTERRUPT_DISABLED                                       = 0b00,
    EOC_INTERRUPT_ENABLED                                        = 0b01,
} ADC_CR1_EOCIE_Type;

#endif /* __ADC_CR1_H__ */