#ifndef __ADC_CR2_H__
#define __ADC_CR2_H__

/**
 * Включить каналы : 
 * встроенный датчик температуры,
 * встроенный источник опорного напряжения
 */
typedef enum
{
    TSVREFE__TEMPERATURE_SENSOR_VREFINT_CHANNEL_DISABLED = 0b00,
    TSVREFE__TEMPERATURE_SENSOR_VREFINT_CHANNEL_ENABLED  = 0b01,
} ADC_CR2_TSVREFE_Type;

/**
 * Если стоит программный запуск -
 * запустить регулярные преобразования
 */
typedef enum
{
    SWSTART__REGULAR_CHANNELS_RESET                      = 0b0,
    SWSTART__REGULAR_CHANNELS_STARTS                     = 0b1,
} ADC_CR2_SWSTART_Type;

/**
 * Если стоит программный запуск -
 * запустить встраиваемые преобразования
 */
typedef enum
{
    JSWSTART__INJECTED_CHANNELS_RESET                    = 0b0,
    JSWSTART__INJECTED_CHANNELS_STARTS                   = 0b1,
} ADC_CR2_JSWSTART_Type;

/**
 * Установка и выбор источника запуска
 * регулярного преобразования
 */
typedef enum
{
    EXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_DISABLED       = 0b0,
    EXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED        = 0b1,
} ADC_CR2_EXTTRIG_Type;

typedef enum
{
    EXTSEL__TIMER_1_CC1_EVENT                            = 0b000,
    EXTSEL__TIMER_1_CC2_EVENT                            = 0b001,
    EXTSEL__TIMER_1_CC3_EVENT                            = 0b010,
    EXTSEL__TIMER_2_CC2_EVENT                            = 0b011,
    EXTSEL__TIMER_3_TRGO_EVENT                           = 0b100,
    EXTSEL__TIMER_4_CC4_EVENT                            = 0b101,
    EXTSEL__EXTI_LINE_11_OR_TIM8_TRGO_EVENT              = 0b110,
    EXTSEL__SWSTART                                      = 0b111,
} ADC_CR2_EXTSEL_Type;

/**
 * Установка и выбор источника запуска
 * встраиваемого преобразования
 */
typedef enum
{
    JEXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_DISABLED      = 0b00,
    JEXTTRIG__CONVERSION_ON_EXTERNAL_EVENT_ENABLED       = 0b01,
} ADC_CR2_JEXTTRIG_Type;

typedef enum
{
    JEXTSEL__TIMER_1_TRGO_EVENT                          = 0b000,
    JEXTSEL__TIMER_1_CC4_EVENT                           = 0b001,
    JEXTSEL__TIMER_2_TRGO_EVENT                          = 0b010,
    JEXTSEL__TIMER_2_CC1_EVENT                           = 0b011,
    JEXTSEL__TIMER_3_CC4_EVENT                           = 0b100,
    JEXTSEL__TIMER_4_TRGO_EVENT                          = 0b101,
    JEXTSEL__EXTI_LINE15_OR_TIM8_CC4_EVENT               = 0b110,
    JEXTSEL__JSWSTART                                    = 0b111,
} ADC_CR2_JEXTSEL_Type;

/**
 * Выравнивание результатов преобразования
 */
typedef enum
{
    ALIGN__RIGHT_ALIGNMENT                               = 0b0,
    ALIGN__LEFT_ALIGNMENT                                = 0b1,
} ADC_CR2_ALIGN_Type;

/**
 * Использование модуля дма
 * Только для АЦП1 и АЦП3
 */
typedef enum
{
    DMA__DMA_MODE_DISABLED                               = 0b0,
    DMA__DMA_MODE_ENABLED                                = 0b1,
} ADC_CR2_DMA_Type;

/**
 * Сброс калибровки
 */
typedef enum
{
    RSTCAL__CALIBRATION_REGISTER_INITIALIZED             = 0b0,
    RSTCAL__INITIALIZE_CALIBRATION_REGISTER              = 0b1,
} ADC_CR2_RSTCAL_Type;

/**
 * Выбор режима запуска
 */
typedef enum
{
    CONT__SINGLE_CONVERSION_MODE                         = 0b0,
    CONT__CONTINUOUS_CONVERSION_MODE                     = 0b1,
} ADC_CR2_CONT_Type;

/**
 * запуск АЦП
 */
typedef enum
{
    ADON__DISABLE_ADC                                    = 0b0,
    ADON__ENABLE_ADC                                     = 0b1,
} ADC_CR2_ADON_Type;

#endif /* __ADC_CR2_H__ */