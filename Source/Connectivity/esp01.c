/**
 * @file esp01.c
 * @brief ESP32/ESP01 Connectivity Implementation
 *
 * Giao tiếp UART với ESP32/ESP01 WiFi module
 * Dùng UART1 (PA9=TX, PA10=RX) @ 115200 baud
 */

#include "esp01.h"
#include "../../Driver/uart.h"
#include <stddef.h>

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo ESP32 connectivity
 *
 * Khởi tạo UART1 với baudrate 115200 (ESP32 mặc định)
 */
void esp01Init(void) {
    uartInit(UART_PERIPHERAL_1, UART_BAUD_115200);
}

/**
 * @brief Gửi dữ liệu cảm biến lên ESP32
 *
 * Format: "[T,H,MQ2]\n"
 * Dùng uartPrintf để format
 */
void esp01SendReading(int8_t temperature, int8_t humidity, uint16_t mq2_adc) {
    uartPrintf(UART_PERIPHERAL_1, "[%d,%d,%u]\n", temperature, humidity, mq2_adc);
}

/**
 * @brief Gửi dữ liệu air quality lên ESP32
 */
void esp01SendAirQuality(AirQuality_Data_t *data) {
    if (data != NULL) {
        uartPrintf(UART_PERIPHERAL_1, "[%d,%d,%u]\n",
                   data->temperature, data->humidity, data->mq2_adc);
    }
}

/**
 * @brief Gửi giá trị MQ2 ADC thô lên ESP32
 */
void esp01SendMQ2(uint16_t value) {
    uartPrintf(UART_PERIPHERAL_1, "%u\n", value);
}

/**
 * @brief Gửi message tùy chỉnh lên ESP32
 *
 * TODO: Implement đầy đủ với va_list
 */
void esp01Printf(const char *format, ...) {
    (void)format;
}
