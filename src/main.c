
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
   setTicks(0);

   uint32_t lastSwitch = 0;
   uint32_t timeOffset = 0;
   display_state_t displayState;
   uint32_t count = 0;

   pinMode(BUTTONPIN, INPUT);
   uint8_t lastState = digitalRead(BUTTONPIN);

   while (1)
   {  
      uint8_t updateDisplay = 0;

      uint8_t newState = digitalRead(BUTTONPIN);
      if ((newState != lastState) && (!newState)) {
         count++;
      }
      lastState = newState;

      uint32_t now = millis() - timeOffset;
      
      if (0 && now - lastSwitch > 1000) {
         displayState.dots = !displayState.dots;
         lastSwitch = now;
         updateDisplay = 1;
      }
      
      uint32_t seconds = now / 1000;
      displayState.digit2 = (seconds % 60) / 10;
      
      if (displayState.digit3 != count % 10) {
         // displayState.digit3 = seconds % 10;
         displayState.digit3 = count % 10;
         updateDisplay = 1;
         uint32_t minutes = seconds / 60;
         displayState.digit0 = (minutes % 60) / 10;
         displayState.digit1 = minutes % 10;
      }
      
      if (updateDisplay) {
         display(&displayState);
      }
   }

   return 0;
}
