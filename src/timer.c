#include "timer.h"

// Contador de us (base de tempo do sistema).
volatile uint64_t ticks_timer;

uint64_t micros(){
    return ticks_timer;
}

void delayMicros(uint64_t x) {
    uint64_t startTime = micros();
    while (micros() - startTime < x);
}

void __attribute__((interrupt("IRQ"))) interrupt_vector(void) {
  if(IRQ_REG(pending_basic) & (1 << 0)) {
    // Interrupção do timer ARM a cada 1 ms
    TIMER_REG(ack) = 1; // reconhece interrupção
    ticks_timer++;
  }

  // outras interrupções...
}

void timer_init(void) {
  TIMER_REG(load) = 1;          // 1MHz / 1 = 1MHz
  TIMER_REG(pre) = 126;
  TIMER_REG(control) = (1 << 9)    // free-running counter
                     | (1 << 7)    // habilita timer
                     | (1 << 5)    // habilita interrupção
                     | (1 << 1);   // timer de 23 bits
   // habilita a interrupção “básica” 0 (core timer)
  IRQ_REG(enable_basic) = (1 << 0);
}
