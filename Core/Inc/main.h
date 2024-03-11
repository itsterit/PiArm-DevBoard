#ifndef __MAIN_H__
#define __MAIN_H__
#include <stdint.h>
#include <stm32f103xb.h>
#include "GPIO/GPIO.h"
#include "clock_control/clock_control.h"
#include "timer/timer.h"
#include "usart/usart.h"
#include "SimpleLog/SimpleLog.h"

extern timer coil_frequency_timer;
extern timer sampling_timer;
void config_timer(uint32_t tmr_freq, uint16_t frq, uint8_t duty);

#endif /* __MAIN_H__ */