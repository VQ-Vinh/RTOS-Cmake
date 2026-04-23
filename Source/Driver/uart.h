/**
 * @file uart.h
 * @brief UART Driver - Serial Communication
 *
 * Chức năng: Truyền/nhận dữ liệu qua UART
 * Hỗ trợ: UART1, UART2, UART3
 * Baudrate: 9600 - 115200
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

/* ========== Định nghĩa UART ========== */
typedef enum {
    UART_PERIPHERAL_1 = 0,   /* PA9/PA10 */
    UART_PERIPHERAL_2 = 1,   /* PA2/PA3 */
    UART_PERIPHERAL_3 = 2    /* PC10/PC11 */
} UART_Peripheral_t;

/* ========== Baudrate ========== */
typedef enum {
    UART_BAUD_9600   = 9600,
    UART_BAUD_19200  = 19200,
    UART_BAUD_38400  = 38400,
    UART_BAUD_57600  = 57600,
    UART_BAUD_115200 = 115200
} UART_Baudrate_t;

/* ========== Data bits ========== */
typedef enum {
    UART_DATA_BITS_8 = 0,
    UART_DATA_BITS_9 = 1
} UART_DataBits_t;

/* ========== Stop bits ========== */
typedef enum {
    UART_STOP_BITS_1   = 0,
    UART_STOP_BITS_0_5 = 1,
    UART_STOP_BITS_2   = 2,
    UART_STOP_BITS_1_5 = 3
} UART_StopBits_t;

/* ========== Parity ========== */
typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 2,
    UART_PARITY_ODD  = 3
} UART_Parity_t;

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo UART
 * @param uart: UART peripheral (UART1/2/3)
 * @param baudrate: Tốc độ truyền (VD: UART_BAUD_115200)
 */
void uartInit(UART_Peripheral_t uart, UART_Baudrate_t baudrate);

/**
 * @brief Gửi 1 ký tự
 * @param uart: UART peripheral
 * @param ch: Ký tự cần gửi
 */
void uartPutChar(UART_Peripheral_t uart, uint8_t ch);

/**
 * @brief Gửi chuỗi string
 * @param uart: UART peripheral
 * @param str: Chuỗi cần gửi (null-terminated)
 */
void uartPutString(UART_Peripheral_t uart, const char *str);

/**
 * @brief Nhận 1 ký tự (blocking)
 * @param uart: UART peripheral
 * @return: Ký tự nhận được
 */
uint8_t uartGetChar(UART_Peripheral_t uart);

/**
 * @brief Kiểm tra có dữ liệu đang chờ
 * @param uart: UART peripheral
 * @return: 1 = có dữ liệu, 0 = không
 */
uint8_t uartAvailable(UART_Peripheral_t uart);

/**
 * @brief In formatted string (printf-style)
 * @param uart: UART peripheral
 * @param format: Format string
 * @param ...: Arguments
 *
 * Hỗ trợ: %d, %u, %x, %c, %s, %%
 */
void uartPrintf(UART_Peripheral_t uart, const char *format, ...);

#endif /* UART_H */
