#ifndef __MAIN_H__
#define __MAIN_H__
#include <stdint.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"
#include "clock_control/clock_control.h"
#include "timer/timer.h"
#include "usart/usart.h"
#include "SimpleLog/SimpleLog.h"
#include "dma_control/dma_control.h"
#include "ModBus/ModBusRTU.h"
#include "adc/adc.h"
#include "../Src/handlers/system_handler.h"
#define ABS_DIFF(x, y) ((x) > (y) ? ((x) - (y)) : ((y) - (x)))

#define MODBUS_ADDRESS (1)
#define USB_BUFFER_SIZE (0xFF)

extern GPIO led_pin;
extern GPIO gen_freq;
extern GPIO dc_enable;

extern uint16_t cur_fault_delay;
extern GPIO cur_fault;

// Работа с таймерами
void set_generation_timing(uint32_t tmr_freq, uint16_t frq, uint8_t duty);
void set_timer_config();
extern timer coil_frequency_timer;
extern timer sampling_timer;
extern timer buzzer_timer;

// Работа с уарт
void log_out_method(char *str, uint8_t len);
extern usart usb_line;
extern SimpleLog Logger;

// Работа с ДМА 4м и 5м каналом
extern dma_control usb_tx_dma;
extern dma_control usb_rx_dma;
extern dma_control adc_samling_dma;

void usb_as_dma_transmit(uint8_t *msg, int16_t len);
void set_usb_tx_dma_cfg();
void set_usb_rx_dma_cfg();
extern uint8_t usb_buffer[USB_BUFFER_SIZE];

// modbus
extern uint16_t usInputRegisters[MB_INPUT_ADR_MAX];
extern uint16_t usHoldingRegisters[MB_HOLDING_ADR_MAX];
void ModBusTxCallback(uint8_t *DataPtr, int16_t DataSize);
bool ModBusSaveCallback(void);
extern ModBusRTU ModBus;

// Работа с АЦП
typedef enum
{
    BAT_VOLTAGE_ERR,
    DC_VOLTAGE_ERR,
    SYSTEM_OK,
    CHECK_SYSTEM,
} get_system_status;

extern volatile uint16_t ref_voltage;
extern volatile uint16_t coil_current;
extern volatile uint16_t dc_startup;
bool get_core_voltage(uint16_t *ret_data);
bool adc_start_system_monitor(uint16_t rev_mv);
get_system_status system_monitor_handler(uint16_t ref_voltage, uint16_t bat_voltage, uint16_t dc_voltage);

#endif /* __MAIN_H__ */