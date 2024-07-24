#ifndef __GPIO_H__
#define __GPIOH__
#include "bcm.h"

#define GPFSEL0 ((volatile uint32_t *)(GPIO_ADDR + 0x00))

void config_gpio_output(int pin);
void config_gpio_input(int pin);

#define GPLEV0 ((volatile uint32_t *)(GPIO_ADDR + 0x34)) // Endereço de Leitura das portas

uint32_t read_gpio_pin(int pin);

#define GPIO_SET_REG (GPIO_ADDR + 0x1C) // GPSET0 register address
#define GPIO_CLR_REG (GPIO_ADDR + 0x28) // GPCLR0 register address

#define GPCLR1 (GPIO_ADDR + 0x2C)

void setGPIOpin(uint8_t pin);
void clearGPIOpin(uint8_t pin);

#define GPCLR0 (GPIO_ADDR + 0x28)
#endif
