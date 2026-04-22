/**
 * @file dht11.h
 * @brief DHT11 Temperature & Humidity Sensor Driver
 * @note Single-wire communication protocol
 * @note HAL-style API for Custom RTOS
 */

#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

/* Forward declaration */
typedef struct _DHT11_Handle_t DHT11_Handle_t;

/**
 * @brief DHT11 status enum
 */
typedef enum {
    DHT11_OK = 0,
    DHT11_TIMEOUT,
    DHT11_ERROR,
    DHT11_CHECKSUM_MISMATCH,
    DHT11_INVALID_PARAMETER,
    __DHT11_STATUS_TYPEDEF_COUNT__
} DHT11_StatusTypeDef;

/**
 * @brief DHT11 handle structure
 * @note Contains all info needed for DHT11 communication
 */
typedef struct _DHT11_Handle_t {
    uint8_t port;           // GPIO port (0=A, 1=B, 2=C)
    uint8_t pin;            // GPIO pin number (0-15)
    float Temperature;       // Temperature in Celsius
    float Humidity;         // Relative Humidity in %
    DHT11_StatusTypeDef Status;  // Last operation status
} DHT11_Handle_t;

/* Error message array */
extern const char* const DHT11_ErrorMsg[__DHT11_STATUS_TYPEDEF_COUNT__];

/**
 * @brief Initialize DHT11 sensor
 * @param handle: DHT11 handle pointer
 * @param port: GPIO port (0=A, 1=B, 2=C)
 * @param pin: Pin number (0-15)
 * @note Must be called before any other DHT11 operations
 */
void DHT11_Init(DHT11_Handle_t *handle, uint8_t port, uint8_t pin);

/**
 * @brief De-initialize DHT11 sensor
 * @param handle: DHT11 handle pointer
 * @note Releases GPIO pin
 */
void DHT11_DeInit(DHT11_Handle_t *handle);

/**
 * @brief Get error message string
 * @param status: DHT11 status code
 * @return Error message string
 */
const char* DHT11_GetErrorMsg(DHT11_StatusTypeDef status);

/**
 * @brief Read temperature and humidity from DHT11
 * @param handle: DHT11 handle pointer
 * @return DHT11 status (DHT11_OK on success)
 * @note Blocking ~4ms read time
 * @note Disables interrupts during read for timing accuracy
 */
DHT11_StatusTypeDef DHT11_ReadData(DHT11_Handle_t *handle);

/**
 * @brief Read temperature in Celsius
 * @param handle: DHT11 handle pointer
 * @return Temperature in Celsius
 */
float DHT11_ReadTemperatureC(DHT11_Handle_t *handle);

/**
 * @brief Read temperature in Fahrenheit
 * @param handle: DHT11 handle pointer
 * @return Temperature in Fahrenheit
 */
float DHT11_ReadTemperatureF(DHT11_Handle_t *handle);

/**
 * @brief Read relative humidity
 * @param handle: DHT11 handle pointer
 * @return Humidity in %
 */
float DHT11_ReadHumidity(DHT11_Handle_t *handle);

#endif /* DHT11_H */