/**
 * @file esp01_f4.c
 * @brief ESP32 Connectivity Implementation for STM32F4
 * UART1 Interface: PA9=TX (STM32→ESP32), PA10=RX (ESP32→STM32)
 * Baudrate: 115200
 */

#include "esp01_f4.h"
#include "../../Driver_f4/uart_f4.h"

/**
 * @brief Initialize ESP32 connectivity
 * Uses UART1 (PA9=TX, PA10=RX) to communicate with ESP32
 */
void esp01Init(void) {
    // Initialize UART1 at 115200 baud (ESP32 default)
    uartInit(UART_PERIPHERAL_1, UART_BAUD_115200);
}

/**
 * @brief Send raw MQ2 ADC reading to ESP32
 * Format: "4095\n" (just the number)
 */
void esp01SendReading(uint16_t value) {
    uartPrintf(UART_PERIPHERAL_1, "%u\n", value);
}
