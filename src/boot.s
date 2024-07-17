
.global start, delay, enable_irq, task_switch
.text
start:
  /*
   * Vetor de interrupções
   * Deve ser copiado no enderço 0x0000
   */
  ldr pc, _reset
  ldr pc, _undef
  ldr pc, _swi
  ldr pc, _iabort
  ldr pc, _dabort
  nop
  ldr pc, _irq
  ldr pc, _fiq

  _reset:    .word   reset
  _undef:    .word   panic
  _swi:      .word   swi
  _iabort:   .word   panic
  _dabort:   .word   panic
  _irq:      .word   irq
  _fiq:      .word   panic

/*
 * Vetor de reset: início do programa aqui.
 */
reset:
  /*
   * configura os stacks pointers do sistema
   */
  mov r0, #0xd2           // Modo IRQ
  msr cpsr, r0
  ldr sp, =stack_irq

  mov r0, #0xd3           // Modo SVC, interrupções mascaradas
  msr cpsr, r0
  ldr sp, =stack_sys

  // Continua executando no modo supervisor (SVC), interrupções desabilitadas

  /*
   * Move o vetor de interrupções para o endereço 0
   */
  ldr r0, =start
  mov r1, #0x0000
  ldmia r0!, {r2,r3,r4,r5,r6,r7,r8,r9}
  stmia r1!, {r2,r3,r4,r5,r6,r7,r8,r9}
  ldmia r0!, {r2,r3,r4,r5,r6,r7,r8,r9}
  stmia r1!, {r2,r3,r4,r5,r6,r7,r8,r9}

   /*
    * Zera segmento BSS
    */
   ldr r0, =bss_begin
   ldr r1, =bss_end
   mov r2, #0
loop_bss:
   cmp r0, r1
   bge done_bss
   strb r2, [r0], #1
   b loop_bss

done_bss:
  /*
   * Executa a função main
   */
  b system_main

/**
 * Espera tempo definido pelo valor de r0.
 */
delay:
   subs r0, r0, #1
   bne delay
   mov pc, lr

/**
 * Habilita ou desabilita interrupções
 * param r0 0 = desabilita, diferente de zero = habilita
 */
.global enable_irq
enable_irq:
   movs r0, r0
   beq disable
   mrs r0, cpsr
   bic r0, r0, #(1 << 7)
   msr cpsr, r0
   mov pc, lr
disable:
   mrs r0, cpsr
   orr r0, r0, #(1 << 7)
   msr cpsr, r0
   mov pc, lr

/**
 * Trava o processador (panic)
 */
panic:
   wfe
   b panic

/*
 * Tratamento da interrupção de software (chamada de sistema).
 */
swi:
   /*
    * Salva o contexto do task na estrutura apontada por tcb
    */
   cpsid if                   // desabilita interrupções no modo swi (região crítica)
   push {r0}
   ldr r0, =tcb               // &tcb atual
   ldr r0, [r0]
   stmib r0, {r1-r14}^        // registradores do usuário
   str lr, [r0, #60]          // endereço de retorno (pc)
   mrs r1, spsr
   str r1, [r0, #64]         // cpsr
   pop {r1}
   str r1, [r0]               // salva o r0 original
   mov r0, r1                 // parâmetro da chamada de sistema
   bl trata_swi               // função em C
   b task_switch              // retorna ao task (ou a outro task)

/*
 * Tratamento das interrupções.
 */
irq:
   sub lr, lr, #4
   /*
    * Salva o contexto do task na estrutura apontada por tcb
    */
   push {r0}
   ldr r0, =tcb               // &tcb atual
   ldr r0, [r0]
   stmib r0, {r1-r14}^        // registradores do usuário
   str lr, [r0, #60]          // endereço de retorno (pc)
   mrs r1, spsr
   str r1, [r0, #64]          // cpsr
   pop {r1}
   str r1, [r0]               // salva o r0 original
   bl trata_irq               // função em C
   b task_switch              // retorna ao task (ou a outro task)

/*
 * Restaura o contexto original e retorna para o task apontado por tcb.
 */
task_switch:
   ldr r0, =tcb               // &tcb atual
   ldr r0, [r0]
   ldr r1, [r0, #64]          // spsr 
   msr spsr, r1
   ldmib r0, {r1-r14}^        // registradores do usuário
   ldr lr, [r0, #60]          // endereço de retorno
   ldr r0, [r0]               // r0 original
   movs pc, lr                // retorna, mudando o modo
   
