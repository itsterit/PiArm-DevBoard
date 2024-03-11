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

extern timer coil_frequency_timer;
extern timer sampling_timer;
void config_timer(uint32_t tmr_freq, uint16_t frq, uint8_t duty);

void log_out_method(char *str, uint8_t len);
extern usart usb_line;
extern SimpleLog Logger;

#endif /* __MAIN_H__ */