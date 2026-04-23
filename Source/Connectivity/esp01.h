/**
 * @file esp01.h
 * @brief ESP32/ESP01 Connectivity - Giao tiếp WiFi module qua UART
 *
 * Kết nối: STM32F1 UART1 (PA9-TX, PA10-RX) ↔ ESP32/ESP01
 * Baudrate: 115200
 */

#ifndef ESP01_H
#define ESP01_H

#include <stdint.h>
#include "../Air_quality/air_quality.h"

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo ESP32 (UART1, 115200 baud)
 * Pins: PA9 = TX, PA10 = RX
 */
void esp01Init(void);

/**
 * @brief Gửi dữ liệu cảm biến lên ESP32
 * @param temperature: Nhiệt độ (°C)
 * @param humidity: Độ ẩm (%RH)
 * @param mq2_adc: Giá trị ADC MQ2 (0-4095)
 *
 * Format gửi: "[T,H,MQ2]\n"
 * Ví dụ: "[25,65,2048]\n"
 */
void esp01SendReading(int8_t temperature, int8_t humidity, uint16_t mq2_adc);

/**
 * @brief Gửi dữ liệu air quality lên ESP32
 * @param data: Con trỏ AirQuality_Data_t
 *
 * Format gửi: "[T,H,MQ2]\n"
 * Ví dụ: "[25,65,2048]\n"
 */
void esp01SendAirQuality(AirQuality_Data_t *data);

/**
 * @brief Gửi giá trị MQ2 ADC thô lên ESP32
 * @param value: Giá trị ADC (0-4095)
 *
 * Format gửi: "4095\n"
 */
void esp01SendMQ2(uint16_t value);

/**
 * @brief Gửi message tùy chỉnh lên ESP32
 * @param format: Format string (kiểu printf)
 * @param ...: Arguments
 *
 * Ví dụ: esp01Printf("TEMP:%d\n", temperature);
 */
void esp01Printf(const char *format, ...);

#endif /* ESP01_H */
