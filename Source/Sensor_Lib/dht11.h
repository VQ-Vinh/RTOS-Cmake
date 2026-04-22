/**
 * @file dht11.h
 * @brief DHT11 Temperature & Humidity Sensor Driver
 * @note Single-wire communication protocol
 */

#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>
#include "../Driver/gpio.h"

/**
 * @brief DHT11 data structure
 */
typedef struct {
    int8_t temperature;     // Temperature in Celsius (0-50°C)
    int8_t humidity;       // Relative Humidity (20-90% RH)
    uint8_t checksum;       // Checksum for data validation
} DHT11_Data_t;

/**
 * @brief Initialize DHT11 sensor
 * @param port: GPIO port (GPIO_PORT_A, GPIO_PORT_B, GPIO_PORT_C)
 * @param pin: Pin number (0-15)
 * @note Uses the specified pin for single-wire communication
 */
void dht11Init(GPIO_Port_t port, uint8_t pin);

/**
 * @brief Read temperature and humidity from DHT11
 * @param data: Pointer to DHT11_Data_t to store readings
 * @return 0 on success, -1 on error (checksum mismatch or timeout)
 * @note Blocking ~4ms read time
 */
int8_t dht11Read(DHT11_Data_t *data);

#endif /* DHT11_H */
