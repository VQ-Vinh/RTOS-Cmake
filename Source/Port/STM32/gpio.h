/**
 * @file gpio.h
 * @brief STM32F1 GPIO Driver
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/* GPIO Pin Mode definitions */
typedef enum {
    GPIO_MODE_INPUT     = 0x0,
    GPIO_MODE_OUTPUT_10MHZ = 0x1,
    GPIO_MODE_OUTPUT_2MHZ  = 0x2,
    GPIO_MODE_OUTPUT_50MHZ = 0x3
} GPIO_Mode_t;

/* GPIO Configuration definitions */
typedef enum {
    GPIO_CNF_ANALOG       = 0x0,
    GPIO_CNF_FLOATING     = 0x1,
    GPIO_CNF_PUPD         = 0x2,
    GPIO_CNF_RESERVED     = 0x3,
    GPIO_CNF_PUSHPULL     = 0x0,
    GPIO_CNF_OPENDRAIN    = 0x1,
    GPIO_CNF_ALT_PUSHPULL = 0x2,
    GPIO_CNF_ALT_OPENDRAIN = 0x3
} GPIO_Cnf_t;

/* GPIO Port definitions */
typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2
} GPIO_Port_t;

/**
 * @brief Initialize GPIO pin
 * @param port: GPIO port (GPIO_PORT_A, GPIO_PORT_B, GPIO_PORT_C)
 * @param pin: Pin number (0-15)
 * @param mode: Output mode
 * @param cnf: Configuration
 */
void gpioInitPin(GPIO_Port_t port, uint8_t pin, GPIO_Mode_t mode, GPIO_Cnf_t cnf);

/**
 * @brief Write to GPIO pin
 * @param port: GPIO port
 * @param pin: Pin number
 * @param state: 0 = low, non-zero = high
 */
void gpioWritePin(GPIO_Port_t port, uint8_t pin, uint8_t state);

/**
 * @brief Toggle GPIO pin
 * @param port: GPIO port
 * @param pin: Pin number
 */
void gpioTogglePin(GPIO_Port_t port, uint8_t pin);

/**
 * @brief Read GPIO pin
 * @param port: GPIO port
 * @param pin: Pin number
 * @return: 0 or 1
 */
uint8_t gpioReadPin(GPIO_Port_t port, uint8_t pin);

#endif /* GPIO_H */