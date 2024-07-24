#ifndef __TIMER_H__
#define __TIMER_H__

#include "bcm.h"
#include <stdint.h>

void initTimer(void);
void handleTimerInterrupt(void);

// Time since boot in us
uint64_t micros();

// Wait for us
void delayMicros(uint64_t);
#endif
