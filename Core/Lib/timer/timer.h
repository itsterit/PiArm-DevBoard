#ifndef __TIMER_H__
#define __TIMER_H__
#include <stm32f103xb.h>

/**
 *  set_channel_output_config
 */
typedef enum
{
    OUTPUT_COMPARE_CLEAR_ENABLE = 0b01,
    OUTPUT_COMPARE_CLEAR_DISABLE = 0b00,
} OUTPUT_COMPARE_CLEAR_Type;

typedef enum
{
    OUTPUT_COMPARE_PRELOAD_ENABLE = 0b01,
    OUTPUT_COMPARE_PRELOAD_DISABLE = 0b00,
} OUTPUT_COMPARE_PRELOAD_Type;

typedef enum
{
    OUTPUT_COMPARE_FAST_ENABLE = 0b01,
    OUTPUT_COMPARE_FAST_DISABLE = 0b00,
} OUTPUT_COMPARE_FAST_Type;

typedef enum
{
    CHANNEL_FROZEN = 0b000,
    CHANNEL_ACTIVE_LEVEL_ON_MATCH = 0b001,
    CHANNEL_INACTIVE_LEVEL_ON_MATCH = 0b010,
    CHANNEL_TOGGLE = 0b011,
    CHANNEL_FORCE_INACTIVE_LEVEL = 0b100,
    CHANNEL_FORCE_ACTIVE_LEVEL = 0b101,
    CHANNEL_PWM_MODE_1 = 0b110,
    CHANNEL_PWM_MODE_2 = 0b111,
} OUTPUT_COMPARE_MODE_Type;

/**
 *  set_event_generation
 */
typedef enum
{
    TRIGGER_GENERATION_ENABLE = 0b01,
    TRIGGER_GENERATION_DISABLE = 0b00,
} TRIGGER_GENERATION_Type;

typedef enum
{
    UPDATE_GENERATION_ENABLE = 0b01,
    UPDATE_GENERATION_DISABLE = 0b00,
} UPDATE_GENERATION_Type;

/**
 * set_dma_interrupt_config
 */
typedef enum
{
    TRIGGER_DMA_REQUEST_ENABLE = 0b01,
    TRIGGER_DMA_REQUEST_DISABLE = 0b00,
} TRIGGER_DMA_REQUEST_Type;

typedef enum
{
    UPDATE_DMA_REQUEST_ENABLE = 0b01,
    UPDATE_DMA_REQUEST_DISABLE = 0b00,
} UPDATE_DMA_REQUEST_Type;

typedef enum
{
    TRIGGER_INTERRUPT_ENABLE = 0b01,
    TRIGGER_INTERRUPT_DISABLE = 0b00,
} TRIGGER_INTERRUPT_Type;

typedef enum
{
    UPDATE_INTERRUPT_ENABLE = 0b01,
    UPDATE_INTERRUPT_DISABLE = 0b00,
} UPDATE_INTERRUPT_Type;

/**
 * slave_mode_control
 */
typedef enum
{
    INTERNAL_TRIGGER0 = 0b000,
    INTERNAL_TRIGGER1 = 0b001,
    INTERNAL_TRIGGER2 = 0b010,
    INTERNAL_TRIGGER3 = 0b011,
    TI1_EDGE_DETECTOR = 0b100,
    FILTERED_TIMER_INPUT1 = 0b101,
    FILTERED_TIMER_INPUT2 = 0b110,
    EXTERNAL_TRIGGER_INPUT = 0b111,
} TRIGGER_SELECTION_Type;

typedef enum
{
    SLAVE_MODE_DISABLED = 0b000,
    ENCODER_MODE1 = 0b001,
    ENCODER_MODE2 = 0b010,
    ENCODER_MODE3 = 0b011,
    RESET_MODE = 0b100,
    GATED_MODE = 0b101,
    TRIGGER_MODE = 0b110,
    EXTERNAL_CLOCK_MODE1 = 0b111,
} SLAVE_MODE_SELECTION_Type;

/**
 * master_mode_config
 */
typedef enum
{
    MASTER_MODE_RESET = 0b000,
    MASTER_MODE_ENABLE = 0b001,
    MASTER_MODE_UPDATE = 0b010,
    MASTER_MODE_COMPARE_PULSE = 0b011,
    MASTER_MODE_COMPARE_OC1REF = 0b100,
    MASTER_MODE_COMPARE_OC2REF = 0b101,
    MASTER_MODE_COMPARE_OC3REF = 0b110,
    MASTER_MODE_COMPARE_OC4REF = 0b111,
} MASTER_MODE_SELECTION_Type;

/**
 * set_counter_config
 */
typedef enum
{
    ARR_REGISTER_NOT_BUFFERED = 0b00,
    ARR_REGISTER_BUFFERED = 0b01,
} AUTO_RELOAD_PRELOAD_Type;

typedef enum
{
    COUNTER_UPCOUNTER = 0b00,
    COUNTER_DOWNCOUNTER = 0b01,
} COUNTER_DIRECTION_Type;

typedef enum
{
    ONE_PULSE_DISABLE = 0b00,
    ONE_PULSE_ENABLE = 0b01,
} ONE_PULSE_MODE_Type;

typedef enum
{
    COUNTER_DISABLE = 0b00,
    COUNTER_ENABLE = 0b01,
} COUNTER_ENABLE_Type;

class timer
{
private:
    TIM_TypeDef *act_timer{0};

public:
    timer(TIM_TypeDef *timer);

    void set_channel_output_config(uint8_t channel,
                                   OUTPUT_COMPARE_CLEAR_Type output_compare_clear,
                                   OUTPUT_COMPARE_PRELOAD_Type output_compare_preload,
                                   OUTPUT_COMPARE_FAST_Type output_compare_fast,
                                   OUTPUT_COMPARE_MODE_Type output_mode);

    void set_event_generation(TRIGGER_GENERATION_Type trigger_generation,
                              UPDATE_GENERATION_Type update_generation,
                              uint32_t capture_compare_generation_msk);

    void set_dma_interrupt_config(TRIGGER_DMA_REQUEST_Type trigger_dma_request,
                                  UPDATE_DMA_REQUEST_Type update_dma_request,
                                  TRIGGER_INTERRUPT_Type trigger_interrupt,
                                  UPDATE_INTERRUPT_Type update_interrupt,
                                  uint32_t capture_compare_dma_request_enable_msk,
                                  uint32_t capture_compare_interrupt_enable_msk);

    void slave_mode_control(TRIGGER_SELECTION_Type trigger_selection, SLAVE_MODE_SELECTION_Type slave_mode_selection);

    void master_mode_config(MASTER_MODE_SELECTION_Type master_mode);

    void capture_compare_register(uint32_t channel_output_polarity_inverted_msk, uint32_t channel_output_enable_msk);

    void set_counter_config(AUTO_RELOAD_PRELOAD_Type auto_reload_preload, COUNTER_DIRECTION_Type counter_direction, ONE_PULSE_MODE_Type one_pulse_mode, COUNTER_ENABLE_Type counter_enable);

    void set_timer_config(uint16_t capture_compare_register1_val,
                          uint16_t capture_compare_register2_val,
                          uint16_t capture_compare_register3_val,
                          uint16_t capture_compare_register4_val,
                          uint16_t auto_reload_register, uint16_t prescaler_val, uint16_t counter_val);
};

#endif