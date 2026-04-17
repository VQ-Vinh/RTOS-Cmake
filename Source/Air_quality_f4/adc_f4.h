/**
 * @file adc_f4.h
 * @brief STM32F4 ADC Driver for MQ2 Gas Sensor
 */

#ifndef ADC_F4_H
#define ADC_F4_H

#include <stdint.h>

/**
 * @brief Initialize ADC1 on PA0 (Channel 0) for MQ2 sensor
 * Connection: MQ2_A0 → PA0 → ADC1_CH0
 */
void adcInit(void);

/**
 * @brief Read ADC value from MQ2 sensor
 * @return: ADC value (0-4095 for 12-bit)
 */
uint16_t adcRead(void);

#endif /* ADC_F4_H */
