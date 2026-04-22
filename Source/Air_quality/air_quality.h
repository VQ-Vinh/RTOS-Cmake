/**
 * @file air_quality.h
 * @brief Air Quality Sensor Library - Unified interface for MQ2 and DHT11
 */

#ifndef AIR_QUALITY_H
#define AIR_QUALITY_H

#include <stdint.h>

/**
 * @brief Combined air quality data structure
 */
typedef struct {
    int8_t temperature;     // Temperature in Celsius (DHT11: 0-50°C)
    int8_t humidity;        // Relative Humidity %RH (DHT11: 20-90%)
    uint16_t mq2_adc;       // MQ2 ADC value (0-4095 for 12-bit)
    uint8_t dht11_error;    // DHT11 error flag (0=ok, 1=error)
} AirQuality_Data_t;

/**
 * @brief Initialize all air quality sensors
 * - MQ2 gas sensor on PB0 (ADC channel 8)
 * - DHT11 temperature & humidity on PA0
 */
void airQualityInit(void);

/**
 * @brief Read all air quality sensors
 * @param data: Pointer to AirQuality_Data_t to store readings
 * @return 0 on success, -1 on DHT11 error
 *
 * Reads:
 * - MQ2 gas sensor ADC (12-bit, 0-4095)
 * - DHT11 temperature (°C) and humidity (%RH)
 */
int8_t airQualityRead(AirQuality_Data_t *data);

/**
 * @brief Get MQ2 gas sensor raw ADC value
 * @return ADC value (0-4095)
 */
uint16_t airQualityGetMQ2(void);

/**
 * @brief Get DHT11 temperature
 * @param data: Pointer to AirQuality_Data_t from previous read
 * @return Temperature in Celsius
 */
int8_t airQualityGetTemperature(AirQuality_Data_t *data);

/**
 * @brief Get DHT11 humidity
 * @param data: Pointer to AirQuality_Data_t from previous read
 * @return Humidity %RH
 */
int8_t airQualityGetHumidity(AirQuality_Data_t *data);

#endif /* AIR_QUALITY_H */
