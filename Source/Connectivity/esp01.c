/**
 * @file esp01.c
 * @brief ESP32/ESP01 Connectivity Implementation
 * UART1 Interface: PA9=TX (STM32→ESP32), PA10=RX (ESP32→STM32)
 * Baudrate: 115200
 */

#include "esp01.h"
#include "../../Driver/uart.h"
#include <stddef.h>

/**
 * @brief Initialize ESP32 connectivity
 * Uses UART1 (PA9=TX, PA10=RX) to communicate with ESP32
 */
void esp01Init(void) {
    // Initialize UART1 at 115200 baud (ESP32 default)
    uartInit(UART_PERIPHERAL_1, UART_BAUD_115200);
}

/**
 * @brief Send sensor data to ESP32
 * Format: "[temp,humidity,mq2]\n"
 * Example: "[25,65,2048]\n"
 */
void esp01SendReading(int8_t temperature, int8_t humidity, uint16_t mq2_adc) {
    uartPrintf(UART_PERIPHERAL_1, "[%d,%d,%u]\n", temperature, humidity, mq2_adc);
}

/**
 * @brief Send air quality data to ESP32
 * Format: "[temp,humidity,mq2]\n"
 */
void esp01SendAirQuality(AirQuality_Data_t *data) {
    if (data != NULL) {
        uartPrintf(UART_PERIPHERAL_1, "[%d,%d,%u]\n",
                   data->temperature, data->humidity, data->mq2_adc);
    }
}

/**
 * @brief Send raw MQ2 ADC reading to ESP32
 */
void esp01SendMQ2(uint16_t value) {
    uartPrintf(UART_PERIPHERAL_1, "%u\n", value);
}

/**
 * @brief Send formatted message to ESP32 (wrapper for uartPrintf)
 */
void esp01Printf(const char *format, ...) {
    (void)format;
}
