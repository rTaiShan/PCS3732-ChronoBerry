#include "bcm.h"
#include <stdint.h>

#define TIMER_ADDR (PERIPH_BASE + 0x00B400)
typedef struct {
  uint32_t load;       // valor de recarga, decrementado até zero
  uint32_t value;      // valor atual, entre ‘load’ e zero
  uint32_t control;    // configuração do timer
  uint32_t ack;        // reconhecimento da interrupção
  uint32_t raw_irq;    // indica interrupção pendente
  uint32_t masked_irq; // indica interrupção ativa
  uint32_t reload;     // valor de recarga, não imediato
  uint32_t pre;        // prescaler para 1MHz (126)
  uint32_t counter;    // valor atual do contador free-running
} timer_reg_t;
#define TIMER_REG(X) ((timer_reg_t*)(TIMER_ADDR))->X

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
volatile uint64_t ticks;

void __attribute__((interrupt("IRQ"))) interrupt_vector(void) {
  if(IRQ_REG(pending_basic) & (1 << 0)) {
    // Interrupção do timer ARM a cada 1 ms
    TIMER_REG(ack) = 1; // reconhece interrupção
    ticks++;
  }

  // outras interrupções...
}