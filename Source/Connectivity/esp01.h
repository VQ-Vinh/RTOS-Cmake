/**
 * @file esp01.h
 * @brief ESP32/ESP01 Connectivity via UART
 * Connection: STM32F1 UART1 (PA9-TX, PA10-RX) ↔ ESP32/ESP01 (115200 baud)
 */

#ifndef ESP01_H
#define ESP01_H

#include <stdint.h>
#include "../Air_quality/air_quality.h"

/**
 * @brief Initialize ESP32 connectivity (UART1 at 115200 baud)
 * Pins: PA9 = TX, PA10 = RX
 */
void esp01Init(void);

/**
 * @brief Send sensor data to ESP32
 * @param temperature: Temperature in Celsius
 * @param humidity: Relative Humidity (%RH)
 * @param mq2_adc: MQ2 ADC reading (0-4095)
 *
 * Format sent: "T:xx,H:xx,MQ2:xxxx\n"
 * Example: "T:25,H:65,MQ2:2048\n"
 */
void esp01SendReading(int8_t temperature, int8_t humidity, uint16_t mq2_adc);

/**
 * @brief Send air quality data to ESP32
 * @param data: Pointer to AirQuality_Data_t
 *
 * Format sent: "T:xx,H:xx,MQ2:xxxx\n"
 * Example: "T:25,H:65,MQ2:2048\n"
 */
void esp01SendAirQuality(AirQuality_Data_t *data);

/**
 * @brief Send raw MQ2 ADC reading to ESP32
 * @param value: ADC reading from MQ2 (0-4095)
 *
 * Format sent: "4095\n"
 */
void esp01SendMQ2(uint16_t value);

/**
 * @brief Send custom message to ESP32
 * @param format: printf-style format string
 * @param ...: Arguments
 *
 * Example: esp01Printf("TEMP:%d\n", temperature);
 */
void esp01Printf(const char *format, ...);

#endif /* ESP01_H */
