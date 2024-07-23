
#include "bcm.h"
#include "timer.h"
#include "GPIO.h"

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
   enable_irq(1);

   // GPIO Usage example
   config_gpio_input(17);
   config_gpio_output(47);
   setGPIOpin(26);

   while (1) {
      if(read_gpio_pin(17)){
         setGPIOpin(26);
      } else {
         setGPIOpin(26);   
      }
   }

   return 0;
}
