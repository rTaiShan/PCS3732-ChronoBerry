
#include <stdint.h>
#include "GPIO.h"

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

void pinMode(uint8_t pin, uint8_t mode) {
    /*
    * Configura o pin da GPIO como saída (001) ou entrada (000)
    */

    uint8_t reg_num = pin/10;
    uint8_t offset = 3 * (pin%10);

    // Estado atual da configuração:
    uint32_t fsel = GPIO_REG(gpfsel[reg_num]);

    if (mode == OUTPUT) {
        GPIO_REG(gpfsel[reg_num]) = (fsel & (~(0x7 << offset))) | (1 << (0x7 << offset));
    } else if (mode == INPUT) {
        GPIO_REG(gpfsel[reg_num]) = (fsel & (~(0x7 << offset))) | (0 << (0x7 << offset));
    }
}

// TODO Proper digitalWrite
void digitalWrite(uint8_t pin, uint8_t val) {
    uint8_t reg = pin % 32
    if (val == HIGH){
        GPIO_REG(gpset[0]) = 0x01 << pin;
    } else if (val == LOW){
        GPIO_REG(gpclr[0]) = 0x01 << pin;
    }
}

// TODO Proper digitalRead
uint8_t digitalRead(uint8_t pin) {
    // lê o estado do GPIO 17
    int gpio17 = GPIO_REG(gplev[0]) & (0x01 << 17);
    return 1;
}
