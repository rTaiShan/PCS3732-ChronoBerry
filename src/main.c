
#include "bcm.h"
#include "timer.h"
#include "GPIO.h"
#include "display.h"

#define BUTTONPIN 0 

// Entry point for interrupt services
void __attribute__((interrupt("IRQ"))) 
handleIrq(void) {
   if(bit_is_set(IRQ_REG(pending_basic), 0)) {
      handleTimerInterrupt();
   }
}

int main(void) 
{
   initTimer();
   initDisplay();
   enable_irq(1);

   while (1) {
      display_state_t displayState = {
         .digit0 = 1,
         .digit1 = 2,         
         .digit2 = 3,
         .digit3 = 4,
         .dots = 1
      };
      display(&displayState);
   }

   return 0;
}
