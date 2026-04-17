/**
 * @file uart_f4.h
 * @brief STM32F4 UART Driver
 */

#ifndef UART_F4_H
#define UART_F4_H

#include <stdint.h>

/* UART peripheral definitions */
typedef enum {
    UART_PERIPHERAL_1 = 0,
    UART_PERIPHERAL_2 = 1,
    UART_PERIPHERAL_3 = 2
} UART_Peripheral_t;

/* UART Baudrate options */
typedef enum {
    UART_BAUD_9600   = 9600,
    UART_BAUD_19200  = 19200,
    UART_BAUD_38400  = 38400,
    UART_BAUD_57600  = 57600,
    UART_BAUD_115200 = 115200
} UART_Baudrate_t;

/**
 * @brief Initialize UART peripheral
 * @param uart: UART peripheral (UART1, UART2, UART3)
 * @param baudrate: Baudrate
 */
void uartInit(UART_Peripheral_t uart, UART_Baudrate_t baudrate);

/**
 * @brief Send single character
 */
void uartPutChar(UART_Peripheral_t uart, uint8_t ch);

/**
 * @brief Send string
 */
void uartPutString(UART_Peripheral_t uart, const char *str);

/**
 * @brief Receive character (blocking)
 */
uint8_t uartGetChar(UART_Peripheral_t uart);

/**
 * @brief Check if data is available
 */
uint8_t uartAvailable(UART_Peripheral_t uart);

/**
 * @brief Send formatted string (simple printf)
 */
void uartPrintf(UART_Peripheral_t uart, const char *format, ...);

#endif /* UART_F4_H */
