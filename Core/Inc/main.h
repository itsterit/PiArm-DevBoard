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

#define INVERT_GENERATOR_SIGNAL (1)
#define MODBUS_ADDRESS          (1)
#define USB_BUFFER_SIZE         (0xFFF)

extern GPIO led_pin;
extern GPIO gen_freq;

extern uint8_t cur_fault_delay;
extern GPIO cur_fault;

// Работа с таймерами
void set_generation_timing(uint32_t tmr_freq, uint16_t frq, uint8_t duty);
void set_timer_config();
extern timer coil_frequency_timer;
extern timer sampling_timer;

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
extern ModBusRTU ModBus;

#endif /* __MAIN_H__ */