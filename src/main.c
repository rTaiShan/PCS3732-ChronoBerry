
#include "bcm.h"
#include "timer.h"
#include "GPIO.h"
#include "display.h"

#define BUTTONPIN 17
#define LED_BUILTIN 47

#define SHOW_TIME 0
#define ADJUST_LEAST_SIGNIFICANT 1
#define MOST_SIGNIFICANT_INCREMENT 3600000
#define ADJUST_MOST_SIGNIFICANT 2
#define LEAST_SIGNIFICANT_INCREMENT 60000

void displayTime(uint32_t, uint8_t);

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

   uint32_t timeOffset = 0;
   
   // Display dot control
   uint32_t lastDotBlink = 0;
   uint8_t dotEnable = 0;

   uint8_t state = SHOW_TIME;
   uint32_t adjustStartTime;

   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
   pinMode(BUTTONPIN, INPUT);
   uint8_t lastButtonState = !digitalRead(BUTTONPIN);
   uint32_t buttonPressTime;

   while (1)
   {  
      // Handle transition of states using button transition
      uint8_t buttonState = !digitalRead(BUTTONPIN);
      uint8_t buttonPressed = 0;
      uint8_t buttonLongPressed = 0;

      if (buttonState != lastButtonState) {
         if (buttonState) { // Button is pressed
            buttonPressTime = millis();
         } else { // Button is released
            uint32_t pressDuration = millis() - buttonPressTime;
            if (pressDuration < 1000) {
               buttonPressed = 1;
               buttonLongPressed = 0;
            } else {
               buttonLongPressed = 1;
               buttonPressed = 0;
            }
         }
         lastButtonState = buttonState;
      }

      if (buttonLongPressed) { // Handle long press
         switch (state) {
         case SHOW_TIME:
            state = ADJUST_MOST_SIGNIFICANT;
            adjustStartTime = millis();
            timeOffset = adjustStartTime + timeOffset;
            digitalWrite(LED_BUILTIN, HIGH);
            break;

         case ADJUST_MOST_SIGNIFICANT:
            state = ADJUST_LEAST_SIGNIFICANT;
            break;

         case ADJUST_LEAST_SIGNIFICANT:
            state = SHOW_TIME;
            timeOffset -= millis();
            digitalWrite(LED_BUILTIN, LOW);
            break;
         
         default:
            break;
         }
      } else if (buttonPressed) { // Handle short press
         switch (state) {
         uint32_t newTimeOffset;
         case ADJUST_MOST_SIGNIFICANT:
            timeOffset = timeOffset + MOST_SIGNIFICANT_INCREMENT;
            break;

         case ADJUST_LEAST_SIGNIFICANT:
            newTimeOffset = timeOffset + LEAST_SIGNIFICANT_INCREMENT;
            if ((newTimeOffset % MOST_SIGNIFICANT_INCREMENT) < (timeOffset % MOST_SIGNIFICANT_INCREMENT)) {
               newTimeOffset -= MOST_SIGNIFICANT_INCREMENT;
            }
            timeOffset = newTimeOffset;
            break;
         
         default:
            break;
         }
      }

      uint32_t now = millis();
      if (now - lastDotBlink > 1000) {
         dotEnable = !dotEnable;
         lastDotBlink = now;
      }

      switch (state) {
         case SHOW_TIME:
            displayTime(millis() + timeOffset, dotEnable);
            break;
         
         case ADJUST_LEAST_SIGNIFICANT:
            displayTime(timeOffset, dotEnable);
            break;
         
         case ADJUST_MOST_SIGNIFICANT: 
            displayTime(timeOffset, dotEnable);
            break;

         default:
            break;
      }
   }

   return 0;
}

void displayTime(uint32_t time, uint8_t dotEnable) {
   display_state_t displayState;
   uint32_t minutes = time / 60000;
   uint32_t hours = (minutes / 60) % 24;
   
   displayState.digit0 = (hours % 24) / 10;
   displayState.digit1 = hours % 10;
   displayState.digit2 = (minutes % 60) / 10;
   displayState.digit3 = minutes % 10;
   displayState.dots = dotEnable;
   display(&displayState);
}
