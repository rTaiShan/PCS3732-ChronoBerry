#include <stdint.h>
#include "bcm.h"

// símbolos definidos pelo linker:
extern uint8_t stack_user1;
extern uint8_t stack_user2;

// Pontos de entrada dos tasks
int user1_main(void);
int user2_main(void);

#define TIMESLICE          200     // 200 ms por task
volatile uint32_t ticks;           // base de tempo do sistema
volatile uint32_t slice;           // tempo disponível para o próximo task

/**
 * Estrutura do
 * Task control block (TCB).
 */
typedef struct tcb_s {
   // contexto do processo:
   uint32_t regs[17];              // r0-r15, cpsr
} tcb_t;
                                      
/**
 * Lista dos tasks definidos no sistema.
 */
tcb_t tcb_list[2] = { 
   /*
    * Contexto do primeiro thread (main).
    */
   {   
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // r0-r12
      (uint32_t)&stack_user1,                    // sp
      0,                                         // lr inicial
      (uint32_t)user1_main,                      // pc = lr = ponto de entrada
      /*0xc0 |*/ 0x10                            // valor do cpsr (modo usuário, interrupções habilitadas)
   },
   /*
    * Contexto do segundo thread (main2).
    */
   {   
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // r0-r12
      (uint32_t)&stack_user2,                    // sp
      0,                                         // lr inicial
      (uint32_t)user2_main,                      // pc = lr = ponto de entrada
      /*0xc0 |*/ 0x10                            // valor do cpsr (modo usuário, interrupções habilitadas)
   }
};

// Variáveis globais, acessadas em boot.s
volatile int tid;                                // identificador do task atual (0 ou 1)
volatile tcb_t *tcb = &tcb_list[0];              // tcb atual
                                      
/**
 * Chama o kernel com swi, a função "yield" (r0 = 1).
 * Devolve o controle ao sistema executivo, que pode escalar outro thread.
 */
void __attribute__((naked)) yield(void) {
   asm volatile("push {lr}  \n\t"
                "mov r0, #1 \n\t"
                "swi #0     \n\t"
                "pop {pc}");
}

/**
 * Retorna o identificador do thread atual (tid).
 */
int __attribute__((naked)) getpid(void) {
   asm volatile("push {lr}  \n\t"
                "mov r0, #2 \n\t"
                "swi #0     \n\t"
                "pop {pc}");
}

/**
 * Retorna o valor de ticks.
 */
unsigned __attribute__((naked)) getticks(void) {
   asm volatile("push {lr}  \n\t"
                "mov r0, #3 \n\t"
                "swi #0     \n\t"
                "pop {pc}");
}

/**
 * Escalador:
 * Escolhe o próximo thread.
 */
void schedule(void) {
   if(tid == 0) {
      tid = 1;
      tcb = &tcb_list[1];
   } else {
      tid = 0;
      tcb = &tcb_list[0];
   }
}

/**
 * Chamado pelo serviço de interrupção swi.
 * Implementa as chamadas do sistema operacional.
 */
void trata_swi(unsigned op) {
   switch(op) {
      /*
       * yield
       */
      case 1:
         /*
          * Escala o próximo task.
          */
         slice = TIMESLICE;
         schedule();
         break;

      /*
       * getpid
       */
      case 2:
         /*
          * Devolve o tid no r0 do usuário.
          */
         tcb->regs[0] = tid;
         break;

      /*
       * getticks
       */
      case 3:
         /*
          * Devolve o tid no r0 do usuário.
          */
         tcb->regs[0] = ticks;
         break;
   }
}

/**
 * Chamado pelo serviço de interrupção irq.
 * Deve ser a interrupção do core timer.
 */
void trata_irq(void) {
   /*
    * Interrupção do timer a cada 10 ms.
    */
   if(bit_is_set(IRQ_REG(pending_basic), 0)) {
      TIMER_REG(ack) = 1;                       // reconhece a interrupção
      ticks = ticks + 10;                       // atualiza base de tempo
      if(slice > 10) slice = slice - 10;        // verifica timeslice do processo atual
      else {
         /*
          * Fim do timeslice: escala novo task.
          */
         slice = TIMESLICE;
         schedule();
      }
   }

   // outras interrupções aqui...
}

/**
 * Inicialização do escalador.
 */
void sched_init(void) {
   /*
    * Configura o primeiro task.
    */
   tid = 0;
   tcb = &tcb_list[0];
   slice = TIMESLICE;

   /*
    * Configura interrupção do timer.
    */
   TIMER_REG(load) = 10000;            // 1MHz / 10000 = 100 Hz
   TIMER_REG(control) = __bit(9)       // habilita free-running counter
                      | __bit(7)       // habilita timer
                      | __bit(5)       // habilita interrupção
                      | __bit(1);      // timer de 23 bits

   IRQ_REG(enable_basic) = __bit(0);   // habilita interrupção básica 0 (timer)
}
