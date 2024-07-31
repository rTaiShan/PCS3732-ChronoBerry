#include "timer.h"

// Microseconds counter (Time-base for system).
volatile uint32_t ticks = 0;

uint64_t micros(){
    return ticks * 1000;
}

uint32_t millis() {
    return ticks;
}

void delayMicros(uint64_t x) {
    uint64_t startTime = micros();
    while (micros() - startTime < x);
}

void delay(uint32_t x) {
    uint32_t startTime = millis();
    while (millis() - startTime < x);
}

void setTicks(uint32_t x) {
    ticks = x;
}

void handleTimerInterrupt(void) {
    TIMER_REG(ack) = 1;
    ticks++;
}

void initTimer(void) {
    TIMER_REG(load) = 1000;             // 1MHz / 1 = 1MHz
    //TIMER_REG(pre) = 126;
    TIMER_REG(control) = (1 << 9)    // free-running counter
                       | (1 << 7)    // enable timer
                       | (1 << 5)    // enable interruption
                       | (1 << 1);   // 23 bits timer
    // enable basic interrupt 0 (core timer)
    IRQ_REG(enable_basic) = (1 << 0);
}
