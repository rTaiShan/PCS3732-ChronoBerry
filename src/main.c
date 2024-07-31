
#include "bcm.h"
#include "timer.h"
#include "GPIO.h"
#include "display.h"

#define BUTTONPIN 17

// Entry point for interrupt services
void __attribute__((interrupt("IRQ")))
handleIrq(void)
{
   if (bit_is_set(IRQ_REG(pending_basic), 0))
   {
      handleTimerInterrupt();
   }
}

int main(void)
{
   initTimer();
   initDisplay();
   enable_irq(1);

   uint32_t digito0, digito1, digito2, digito3;
   digito0 = digito1 = digito2 = digito3 = 0;
   uint32_t lastSwitch = 0;
   uint32_t timeOffset = 0;

   display_state_t displayState;

   setTicks(0);

   while (1)
   {  
      // uint32_t now = millis() - timeOffset;
      uint32_t now = millis();
      if (now - lastSwitch > 1000) {
         displayState.dots = !displayState.dots;
         lastSwitch = now;
      }
      
      uint32_t seconds = now / 1000;
      displayState.digit2 = (seconds % 60) / 10;
      displayState.digit3 = seconds % 10;
      uint32_t minutes = seconds / 60;
      displayState.digit0 = (minutes % 60) / 10;
      displayState.digit1 = minutes % 10;

      display(&displayState);
   }

   return 0;
}
