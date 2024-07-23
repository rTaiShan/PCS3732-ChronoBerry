
.section .init
.global start
start:
   // Entry point and interrupt vector
   ldr pc, reset_addr
   ldr pc, undef_addr
   ldr pc, swi_addr
   ldr pc, abort1_addr
   ldr pc, abort2_addr
   nop
   ldr pc, irq_addr
   ldr pc, fiq_addr

   reset_addr:  .word   reset
   undef_addr:  .word   panic
   swi_addr:    .word   panic
   abort1_addr: .word   panic
   abort2_addr: .word   panic
   irq_addr:    .word   handleIrq
   fiq_addr:    .word   panic

final:

// Initial instruction (Reset vector).
reset:
   // Check execution privileges. EL2 (HYP) or EL1 (SVC)
   mrs r0, cpsr
   and r0, r0, #0x1f
   cmp r0, #0x1a
   bne continue

   // Leave EL2 (HYP)
   mrs r0, cpsr
   bic r0, r0, #0x1f
   orr r0, r0, #0x13
   msr spsr_cxsf, r0
   add lr, pc, #4       // Points to 'continue' label
   msr ELR_hyp, lr
   eret                 // 'return' from EL2 privilege to EL1

continue:
   // Check CPU indexes
   mrc p15,0,r0,c0,c0,5    // MPIDR register
   ands r0, r0, #0xff
   beq core0

// Cores #1, #2 e #3 execute from here
panic:
   wfe
   b panic

// Core #0 execution
core0:
   // Set up stack pointers
   mov r0, #0xD2     // IRQ
   msr cpsr_c,r0
   mov sp, #0x4000

   mov r0, #0xD3     // SVC
   msr cpsr_c,r0
   mov sp, #0x8000

   // Resume with supervisor mode (SVC), interruptions disabled

   // Copy interruption vector from address 0x8000 to address 0
   ldr r0, =start
   ldr r1, =final
   mov r2, #0x0000
copy:
   ldr r3, [r0], #4
   str r3, [r2], #4
   cmp r0, r1
   blt copy

   b main

// Enable (r0 != 0) or Disable (r0 = 0) interruptions
.global enable_irq
enable_irq:
   movs r0, r0
   beq disable
   mrs r0, cpsr
   bic r0, r0, #0x80
   msr cpsr_c, r0
   mov pc, lr
disable:
   mrs r0, cpsr
   orr r0, r0, #0x80
   msr cpsr_c, r0
   mov pc, lr
