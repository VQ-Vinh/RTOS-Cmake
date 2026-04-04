/**
 * @file gpio.c
 * @brief STM32F4 GPIO Driver Implementation
 */

#include "gpio.h"
#include "rcc.h"

// =============================================================================
// Base Addresses
// =============================================================================
#define GPIOA_BASE      0x40020000
#define GPIOB_BASE      0x40020400
#define GPIOC_BASE      0x40020800
#define GPIOD_BASE      0x40020C00
#define GPIOE_BASE      0x40021000

// =============================================================================
// GPIO Registers (STM32F4)
// =============================================================================
typedef struct {
    volatile uint32_t MODER;   // Mode register
    volatile uint32_t OTYPER;  // Output type register
    volatile uint32_t OSPEEDR; // Output speed register
    volatile uint32_t PUPDR;   // Pull-up/pull-down register
    volatile uint32_t IDR;     // Input data register
    volatile uint32_t ODR;     // Output data register
    volatile uint32_t BSRR;   // Bit set/reset register
    volatile uint32_t LCKR;   // Lock register
    volatile uint32_t AFR[2]; // Alternate function registers (AFRL, AFRH)
} GPIO_Reg_t;

#define GPIOA   ((GPIO_Reg_t *)GPIOA_BASE)
#define GPIOB   ((GPIO_Reg_t *)GPIOB_BASE)
#define GPIOC   ((GPIO_Reg_t *)GPIOC_BASE)
#define GPIOD   ((GPIO_Reg_t *)GPIOD_BASE)
#define GPIOE   ((GPIO_Reg_t *)GPIOE_BASE)

// =============================================================================
// Clock enable bits in AHB1ENR
// =============================================================================
#define RCC_AHB1ENR_GPIOAEN     (1 << 0)
#define RCC_AHB1ENR_GPIOBEN     (1 << 1)
#define RCC_AHB1ENR_GPIOCEN     (1 << 2)
#define RCC_AHB1ENR_GPIODEN     (1 << 3)
#define RCC_AHB1ENR_GPIOEEN     (1 << 4)

static GPIO_Reg_t* const gpioPorts[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};
static const uint32_t rccClocks[] = {RCC_AHB1ENR_GPIOAEN, RCC_AHB1ENR_GPIOBEN,
                                     RCC_AHB1ENR_GPIOCEN, RCC_AHB1ENR_GPIODEN,
                                     RCC_AHB1ENR_GPIOEEN};

void gpioInitPin(GPIO_Port_t port, uint8_t pin, GPIO_Init_t *init) {
    // Enable clock for port
    rccEnableClock(rccClocks[port]);

    GPIO_Reg_t *gpio = gpioPorts[port];

    // Configure MODER (2 bits per pin)
    gpio->MODER &= ~(0x3 << (pin * 2));
    gpio->MODER |= (init->mode << (pin * 2));

    // Configure OTYPER (1 bit per pin)
    gpio->OTYPER &= ~(1 << pin);
    gpio->OTYPER |= (init->otype << pin);

    // Configure OSPEEDR (2 bits per pin)
    gpio->OSPEEDR &= ~(0x3 << (pin * 2));
    gpio->OSPEEDR |= (init->ospeed << (pin * 2));

    // Configure PUPDR (2 bits per pin)
    gpio->PUPDR &= ~(0x3 << (pin * 2));
    gpio->PUPDR |= (init->pupd << (pin * 2));
}

void gpioInitPinSimple(GPIO_Port_t port, uint8_t pin, GPIO_Mode_t mode) {
    GPIO_Init_t init;
    init.mode = mode;
    init.otype = GPIO_OTYPE_PUSHPULL;
    init.ospeed = GPIO_OSPEED_50MHZ;
    init.pupd = GPIO_PUPD_NONE;

    gpioInitPin(port, pin, &init);
}

void gpioWritePin(GPIO_Port_t port, uint8_t pin, uint8_t state) {
    GPIO_Reg_t *gpio = gpioPorts[port];
    if (state) {
        gpio->BSRR = (1 << pin);        // Set bit
    } else {
        gpio->BSRR = (1 << (pin + 16)); // Reset bit
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