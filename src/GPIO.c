
#include <stdint.h>
#include "GPIO.h"


volatile uint32_t* gpio_set = (volatile uint32_t*) GPIO_SET_REG; // Pointer to the GPSET0
volatile uint32_t* gpio_clr = (volatile uint32_t*) GPIO_CLR_REG; // Pointer to the GPCLR0


void config_gpio_output(int pin) {
    asm volatile(
        "push {r1, r2, r3}            \n"
        "ldr r0, =%[gpfsel0]          \n"
        "add r0, r0, %1               \n"
        "ldr r1, [r0]                 \n"
        "mov r2, %2                   \n"
        "mov r3, #7                   \n"
        "lsl r3, r3, r2               \n"
        "bic r1, r1, r3               \n"
        "mov r3, #1                   \n"
        "lsl r3, r3, r2               \n"
        "orr r1, r1, r3               \n"
        "str r1, [r0]                 \n"
        "pop {r1, r2, r3}             \n"
        :
        : [gpfsel0] "i" (GPFSEL0), "r" ((pin / 10) * 4), "r" ((pin % 10) * 3)
        : "r0", "r1", "r2", "r3", "memory"
    );
}

void config_gpio_input(int pin) {
    asm volatile(
        "push {r1, r2, r3}            \n"
        "ldr r0, =%[gpfsel0]          \n"
        "add r0, r0, %1               \n"
        "ldr r1, [r0]                 \n"
        "mov r2, %2                   \n"
        "mov r3, #7                   \n"
        "lsl r3, r3, r2               \n"
        "bic r1, r1, r3               \n"
        "str r1, [r0]                 \n"
        "pop {r1, r2, r3}             \n"
        :
        : [gpfsel0] "i" (GPFSEL0), "r" ((pin / 10) * 4), "r" ((pin % 10) * 3)
        : "r0", "r1", "r2", "r3", "memory"
    );
}

uint32_t read_gpio_pin(int pin) {
    uint32_t value;
    uint32_t mask = 1 << pin;

    asm volatile (
        "ldr %[value], [%[gplev0]]\n"     // Carrega o valor do registrador GPLEV0
        "and %[value], %[value], %[mask]\n" // Mascara o bit específico do pino
        "cmp %[value], #0\n"              // Compara o valor mascarado com 0
        "moveq %[value], #0\n"            // Se o valor for 0, value é 0
        "movne %[value], #1\n"            // Se o valor não for 0, value é 1
        : [value] "=r" (value)
        : [gplev0] "r" (GPLEV0), [mask] "r" (mask)
        : "memory"
    );

    return value;
}


void setGPIOpin(uint8_t pin) {
    asm volatile(
        "mov r0, %[pin]\n"            // Move pin number into r0
        "ldr r1, %[set]\n"            // Load the address of GPIO_SET_REG into r1
        "mov r2, #1\n"                // Move 1 into r2
        "lsl r2, r0\n"                // Left shift r2 by r0 (pin number)
        "str r2, [r1]\n"              // Store r2 into the address contained in r1 (GPIO_SET_REG)
        : 
        : [pin] "r" (pin), [set] "m" (gpio_set) // Input operands
        : "r0", "r1", "r2"            // Clobbered registers
    );
}

void clearGPIOpin(uint8_t pin) {
    asm volatile(
        "mov r0, %[pin]\n"            // Move pin number into r0
        "ldr r1, %[clr]\n"            // Load the address of GPIO_CLR_REG into r1
        "mov r2, #1\n"                // Move 1 into r2
        "lsl r2, r0\n"                // Left shift r2 by r0 (pin number)
        "str r2, [r1]\n"              // Store r2 into the address contained in r1 (GPIO_CLR_REG)
        : 
        : [pin] "r" (pin), [clr] "m" (gpio_clr) // Input operands
        : "r0", "r1", "r2"            // Clobbered registers
    );
}       


int GPIO_test_main() {
    // Exemplo de uso para configurar o pino 17 como entrada
    config_gpio_input(17);
    config_gpio_output(47);
    setGPIOpin(26);

    while (1){
        /*
        if(read_gpio_pin(17)){
            setGPIOpin(26);
        }else {
            setGPIOpin(26);   
        }*/
    }
    return 0;
}

