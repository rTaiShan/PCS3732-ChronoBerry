#pragma once

typedef struct tcb_s tcb_t;

extern volatile int tid;
extern volatile tcb_t *tcb;

void yield(void);
int getpid(void);
unsigned getticks(void);

void schedule(void);
void trata_swi(unsigned);
void trata_irq(void);
void sched_init(void);

