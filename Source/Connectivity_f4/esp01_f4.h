/**
 * @file esp01_f4.h
 * @brief ESP32 Connectivity for STM32F4 via UART
 */

#ifndef ESP01_F4_H
#define ESP01_F4_H

#include <stdint.h>

/**
 * @brief Initialize ESP32 connectivity (UART1 at 115200 baud)
 */
void esp01Init(void);

/**
 * @brief Send raw MQ2 ADC reading to ESP32
 */
void esp01SendReading(uint16_t value);

#endif /* ESP01_F4_H */
