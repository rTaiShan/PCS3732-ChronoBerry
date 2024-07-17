#pragma once

// Definições dos endereços dos registradores
#define PERIPH_BASE 0x3f000000
#define GPIO_BASE (PERIPH_BASE + 0x200000)
#define GPFSEL0 ((volatile uint32_t *)(GPIO_BASE + 0x00))

void config_gpio_output(int pin) ;

void config_gpio_input(int pin);

#define GPLEV0 ((volatile uint32_t *)(GPIO_BASE + 0x34)) // Endereço de Leitura das portas

uint32_t read_gpio_pin(int pin);

#define GPIO_SET_REG (GPIO_BASE + 0x1C) // GPSET0 register address
#define GPIO_CLR_REG (GPIO_BASE + 0x28) // GPCLR0 register address

#define GPCLR1 (GPIO_BASE + 0x2C)

void setGPIOpin(uint8_t pin);
void clearGPIOpin(uint8_t pin);

int GPIO_test_main();

#define GPCLR0 (GPIO_BASE + 0x28)