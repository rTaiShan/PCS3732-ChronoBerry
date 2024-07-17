#pragma once
#include "bcm.h"
#include <stdint.h>

// Time since boot in us
uint64_t micros();

// Wait for us
void delayMicros(uint64_t);

void timer_init(void);