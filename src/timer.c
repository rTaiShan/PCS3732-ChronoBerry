#include "timer.h"

// Microseconds counter (Time-base for system).
volatile uint64_t ticks;

uint64_t micros(){
    return ticks;
}

void delayMicros(uint64_t x) {
    uint64_t startTime = micros();
    while (micros() - startTime < x);
}

void handleTimerInterrupt(void) {
    TIMER_REG(ack) = 1;
    ticks++;
}

void initTimer(void) {
    TIMER_REG(load) = 1;             // 1MHz / 1 = 1MHz
    TIMER_REG(pre) = 126;
    TIMER_REG(control) = (1 << 9)    // free-running counter
                       | (1 << 7)    // enable timer
                       | (1 << 5)    // enable interruption
                       | (1 << 1);   // 23 bits timer
    // enable basic interrupt 0 (core timer)
    IRQ_REG(enable_basic) = (1 << 0);
}
