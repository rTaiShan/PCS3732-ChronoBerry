#ifndef TIMER_PROF_H
#define TIMER_PROF_H

#include "bcm.h"
#include <stdint.h>

// Time since boot in us
uint64_t micros();

// Wait for us
void delayMicros(uint64_t);

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

// Contador de ms (base de tempo do sistema).
volatile uint64_t ticks_timer;

void __attribute__((interrupt("IRQ"))) interrupt_vector(void) {
  if(IRQ_REG(pending_basic) & (1 << 0)) {
    // Interrupção do timer ARM a cada 1 ms
    TIMER_REG(ack) = 1; // reconhece interrupção
    ticks_timer++;
  }

  // outras interrupções...
}

#endif