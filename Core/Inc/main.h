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

// Работа с таймерами
void config_timer(uint32_t tmr_freq, uint16_t frq, uint8_t duty);
extern timer coil_frequency_timer;
extern timer sampling_timer;

// Работа с уарт
void log_out_method(char *str, uint8_t len);
extern usart usb_line;
extern SimpleLog Logger;

#define DMA_MAX_FRAME_SIZE 0xFFFF
extern dma_control usb_tx_dma;
extern dma_control usb_rx_dma;
volatile void usb_as_dma_transmit(uint8_t *msg, int16_t len);

void set_usb_tx_dma_cfg();
void set_usb_rx_dma_cfg();

extern uint8_t usb_buffer[0xFF];

#endif /* __MAIN_H__ */