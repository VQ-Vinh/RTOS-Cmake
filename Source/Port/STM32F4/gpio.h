/**
 * @file gpio.h
 * @brief STM32F4 GPIO Driver
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/* GPIO Pin Mode definitions */
typedef enum {
    GPIO_MODE_INPUT     = 0x0,
    GPIO_MODE_OUTPUT    = 0x1,
    GPIO_MODE_ALT       = 0x2,
    GPIO_MODE_ANALOG    = 0x3
} GPIO_Mode_t;

/* GPIO Output Type */
typedef enum {
    GPIO_OTYPE_PUSHPULL = 0x0,
    GPIO_OTYPE_OPENDRAIN = 0x1
} GPIO_OType_t;

/* GPIO Speed */
typedef enum {
    GPIO_OSPEED_2MHZ   = 0x0,
    GPIO_OSPEED_25MHZ  = 0x1,
    GPIO_OSPEED_50MHZ  = 0x2,
    GPIO_OSPEED_100MHZ = 0x3
} GPIO_OSpeed_t;

/* GPIO Pull-up/Pull-down */
typedef enum {
    GPIO_PUPD_NONE        = 0x0,
    GPIO_PUPD_PULLUP      = 0x1,
    GPIO_PUPD_PULLDOWN    = 0x2,
    GPIO_PUPD_RESERVED   = 0x3
} GPIO_PUPD_t;

/* GPIO Port definitions */
typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2,
    GPIO_PORT_D = 3,
    GPIO_PORT_E = 4
} GPIO_Port_t;

/* GPIO Configuration structure */
typedef struct {
    GPIO_Mode_t   mode;
    GPIO_OType_t  otype;
    GPIO_OSpeed_t ospeed;
    GPIO_PUPD_t   pupd;
} GPIO_Init_t;

/**
 * @brief Initialize GPIO pin
 * @param port: GPIO port (GPIO_PORT_A, GPIO_PORT_B, etc.)
 * @param pin: Pin number (0-15)
 * @param mode: Input/Output/Alt/Analog mode
 */
void gpioInitPin(GPIO_Port_t port, uint8_t pin, GPIO_Init_t *init);

/**
 * @brief Initialize GPIO pin (simple version)
 * @param port: GPIO port
 * @param pin: Pin number
 * @param mode: Output mode
 */
void gpioInitPinSimple(GPIO_Port_t port, uint8_t pin, GPIO_Mode_t mode);

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