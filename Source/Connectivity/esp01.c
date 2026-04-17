/**
 * @file esp01.c
 * @brief ESP32/ESP01 Connectivity Implementation
 * UART1 Interface: PA9=TX (STM32→ESP32), PA10=RX (ESP32→STM32)
 * Baudrate: 115200
 */

#include "esp01.h"
#include "../../Driver/uart.h"

/**
 * @brief Initialize ESP32 connectivity
 * Uses UART1 (PA9=TX, PA10=RX) to communicate with ESP32
 */
void esp01Init(void) {
    // Initialize UART1 at 115200 baud (ESP32 default)
    uartInit(UART_PERIPHERAL_1, UART_BAUD_115200);
}

/**
 * @brief Send MQ2 gas sensor reading to ESP32
 * Format: "MQ2:XXXX\n" where XXXX is ADC value (0-4095)
 */
void esp01SendReading(uint16_t value) {
    uartPrintf(UART_PERIPHERAL_1, "MQ2:%u\n", value);
}

/**
 * @brief Send formatted message to ESP32 (wrapper for uartPrintf)
 */
void esp01Printf(const char *format, ...) {
    // This would require variadic macro support - delegate to uartPrintf
    // For now, use direct uartPrintf in your code
    // Future: implement variadic wrapper if needed
}