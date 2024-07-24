
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
   uint32_t tempo = 100;

   display_state_t displayState = {
       .digit0 = 1,
       .digit1 = 2,
       .digit2 = 3,
       .digit3 = 4,
       .dots = 1};
   uint64_t tempo_antes;

   while (1)
   {
      tempo_antes = micros();

      uint32_t minutos = tempo / 60;
      uint32_t segundos = (tempo - minutos * 60);

      uint32_t unidades_segundos = segundos % 10;
      uint32_t dezenas_segundos = (segundos / 10) % 10;
      uint32_t unidades_minutos = minutos % 10;
      uint32_t dezenas_minutos = (minutos / 10) % 10;

      displayState.digit0 = dezenas_minutos;
      displayState.digit1 = unidades_minutos;
      displayState.digit2 = dezenas_segundos;
      displayState.digit3 = unidades_segundos;

      tempo--;
      display(&displayState);
      delayMicros(1000000 - (micros() - tempo_antes));
   }

   return 0;
}
