#include "timer.h"

uint64_t micros(){
    return ticks;
}

void delayMicros(uint64_t x) {
    uint64_t startTime = micros();
    while (micros() - startTime < x);
}
