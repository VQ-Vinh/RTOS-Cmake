/**
 * @file uart.h
 * @brief STM32F1 UART Driver
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

/* UART peripheral definitions */
typedef enum {
    UART1 = 0,
    UART2 = 1,
    UART3 = 2
} UART_Peripheral_t;

/* UART Baudrate options */
typedef enum {
    UART_BAUD_9600   = 9600,
    UART_BAUD_19200  = 19200,
    UART_BAUD_38400  = 38400,
    UART_BAUD_57600  = 57600,
    UART_BAUD_115200 = 115200
} UART_Baudrate_t;

/* UART Data bits */
typedef enum {
    UART_DATA_BITS_8 = 0,
    UART_DATA_BITS_9 = 1
} UART_DataBits_t;

/* UART Stop bits */
typedef enum {
    UART_STOP_BITS_1   = 0,
    UART_STOP_BITS_0_5 = 1,
    UART_STOP_BITS_2   = 2,
    UART_STOP_BITS_1_5 = 3
} UART_StopBits_t;

/* UART Parity */
typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 2,
    UART_PARITY_ODD  = 3
} UART_Parity_t;

/**
 * @brief Initialize UART peripheral
 * @param uart: UART peripheral (UART1, UART2, UART3)
 * @param baudrate: Baudrate
 */
void uartInit(UART_Peripheral_t uart, UART_Baudrate_t baudrate);

/**
 * @brief Send single character
 * @param uart: UART peripheral
 * @param ch: Character to send
 */
void uartPutChar(UART_Peripheral_t uart, uint8_t ch);

/**
 * @brief Send string
 * @param uart: UART peripheral
 * @param str: String to send
 */
void uartPutString(UART_Peripheral_t uart, const char *str);

/**
 * @brief Receive character (blocking)
 * @param uart: UART peripheral
 * @return: Received character
 */
uint8_t uartGetChar(UART_Peripheral_t uart);

/**
 * @brief Check if data is available
 * @param uart: UART peripheral
 * @return: 1 if data available, 0 otherwise
 */
uint8_t uartAvailable(UART_Peripheral_t uart);

/**
 * @brief Send formatted string (simple printf)
 * @param uart: UART peripheral
 * @param format: Format string
 * @param ...: Arguments
 */
void uartPrintf(UART_Peripheral_t uart, const char *format, ...);

#endif /* UART_H */