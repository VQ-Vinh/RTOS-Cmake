/**
 * @file adc.h
 * @brief STM32F1 ADC Driver for MQ2 Gas Sensor
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/**
 * @brief Initialize ADC1 on PB0 (Channel 8) for MQ2 sensor
 * Connection: MQ2_A0 → PB0 → ADC1_CH8
 */
void adcInit(void);

/**
 * @brief Read ADC value from MQ2 sensor on PB0
 * @return: ADC value (0-4095 for 12-bit)
 * @note: 0 = 0V (no gas), 4095 = 3.3V (maximum gas)
 */
uint16_t adcRead(void);

#endif /* ADC_H */