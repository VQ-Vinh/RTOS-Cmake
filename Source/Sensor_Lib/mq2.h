/**
 * @file mq2.h
 * @brief MQ2 Gas Sensor Library
 * Uses Driver/adc.c for ADC operations
 *
 * TODO: Expand for PPM calculation (sensitivity curve)
 */

#ifndef MQ2_H
#define MQ2_H

#include <stdint.h>

/* MQ2 ADC channel */
#define MQ2_ADC_CHANNEL   8      /* PB0 on STM32F1 */
#define MQ2_ADC_SAMPLE    ADC_SAMPLE_71_5  /* 71.5 cycles sample time */

/**
 * @brief Initialize MQ2 sensor
 * Configures ADC channel 8 (PB0) for MQ2 sensor
 */
void mq2Init(void);

/**
 * @brief Read MQ2 gas sensor raw ADC value
 * @return: ADC value (0-4095 for 12-bit)
 *
 * Note: 0 = 0V (no gas), 4095 = 3.3V (maximum gas concentration)
 */
uint16_t mq2ReadADC(void);

/**
 * @brief Read MQ2 gas concentration
 * @return: Gas concentration (raw ADC value, for now)
 *
 * TODO: Convert to PPM using sensitivity curve
 * Formula: PPM = a * (ratio^b) where ratio = sensorResistance / cleanAirResistance
 */
uint16_t mq2ReadPPM(void);

/**
 * @brief Check if MQ2 is initialized
 * @return: 1 if initialized, 0 if not
 */
uint8_t mq2IsInitialized(void);

#endif /* MQ2_H */
