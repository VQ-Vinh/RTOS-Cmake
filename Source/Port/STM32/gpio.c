/**
 * @file gpio.c
 * @brief STM32F1 GPIO Driver Implementation
 */

#include "gpio.h"
#include "rcc.h"

// =============================================================================
// Base Addresses
// =============================================================================
#define GPIOA_BASE      0x40010800
#define GPIOB_BASE      0x40010C00
#define GPIOC_BASE      0x40011000

// =============================================================================
// GPIO Registers
// =============================================================================
typedef struct {
    volatile uint32_t CRL;      // Control register low (pins 0-7)
    volatile uint32_t CRH;      // Control register high (pins 8-15)
    volatile uint32_t IDR;      // Input data register
    volatile uint32_t ODR;      // Output data register
    volatile uint32_t BSRR;     // Bit set/reset register
    volatile uint32_t BRR;      // Bit reset register
    volatile uint32_t LCKR;     // Lock register
} GPIO_Reg_t;

#define GPIOA   ((GPIO_Reg_t *)GPIOA_BASE)
#define GPIOB   ((GPIO_Reg_t *)GPIOB_BASE)
#define GPIOC   ((GPIO_Reg_t *)GPIOC_BASE)

// =============================================================================
// Clock enable bits in APB2ENR
// =============================================================================
#define RCC_IOPAEN     (1 << 2)
#define RCC_IOPBEN     (1 << 3)
#define RCC_IOPCEN     (1 << 4)

static GPIO_Reg_t* const gpioPorts[] = {GPIOA, GPIOB, GPIOC};
static const uint32_t rccClocks[] = {RCC_IOPAEN, RCC_IOPBEN, RCC_IOPCEN};

void gpioInitPin(GPIO_Port_t port, uint8_t pin, GPIO_Mode_t mode, GPIO_Cnf_t cnf) {
    // Enable clock for port
    rccEnableClock(rccClocks[port]);

    GPIO_Reg_t *gpio = gpioPorts[port];
    volatile uint32_t *cr = (pin < 8) ? &gpio->CRL : &gpio->CRH;
    uint8_t shift = (pin % 8) * 4;

    // Clear CNF and MODE bits
    *cr &= ~(0xF << shift);
    // Set new configuration
    *cr |= ((cnf << 2) | mode) << shift;
}

void gpioWritePin(GPIO_Port_t port, uint8_t pin, uint8_t state) {
    GPIO_Reg_t *gpio = gpioPorts[port];
    if (state) {
        gpio->ODR |= (1 << pin);
    } else {
        gpio->ODR &= ~(1 << pin);
    }
}

void gpioTogglePin(GPIO_Port_t port, uint8_t pin) {
    GPIO_Reg_t *gpio = gpioPorts[port];
    gpio->ODR ^= (1 << pin);
}

uint8_t gpioReadPin(GPIO_Port_t port, uint8_t pin) {
    GPIO_Reg_t *gpio = gpioPorts[port];
    return (gpio->IDR >> pin) & 0x1;
}