#ifndef __TIMER_H__
#define __TIMER_H__

#include "bcm.h"
#include <stdint.h>

void initTimer(void);
void handleTimerInterrupt(void);
void setTicks(uint32_t);

// Time since boot in ms
uint32_t millis();
// Time since boot in us
uint64_t micros();

// Wait for ms
void delay(uint32_t);
// Wait for us
void delayMicros(uint64_t);
#endif
