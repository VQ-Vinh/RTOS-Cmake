/**
 * @file esp01.h
 * @brief ESP32/ESP01 Connectivity via UART
 * Connection: STM32F1 UART1 (PA9-TX, PA10-RX) ↔ ESP32/ESP01 (115200 baud)
 */

#ifndef ESP01_H
#define ESP01_H

#include <stdint.h>

/**
 * @brief Initialize ESP32 connectivity (UART1 at 115200 baud)
 * Pins: PA9 = TX, PA10 = RX
 */
void esp01Init(void);

/**
 * @brief Send raw MQ2 ADC reading to ESP32
 * @param value: ADC reading from MQ2 (0-4095)
 * 
 * Format sent: "4095\n"
 * Just the number without any prefix, continuous stream
 */
void esp01SendReading(uint16_t value);

/**
 * @brief Send custom message to ESP32
 * @param format: printf-style format string
 * @param ...: Arguments
 * 
 * Example: esp01Printf("TEMP:%d\n", temperature);
 */
void esp01Printf(const char *format, ...);

#endif /* ESP01_H */