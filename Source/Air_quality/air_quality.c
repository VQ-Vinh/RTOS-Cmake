/**
 * @file air_quality.c
 * @brief Air Quality Sensor Library Implementation
 * Combines: MQ2 gas sensor + DHT11 temperature & humidity
 *
 * Uses Sensor_Lib/mq2.c and Sensor_Lib/dht11.c
 */

#include "air_quality.h"
#include "../Sensor_Lib/mq2.h"
#include "../Sensor_Lib/dht11.h"
#include "../Driver/gpio.h"
#include <stddef.h>

/* DHT11 handle for internal use */
static DHT11_Handle_t dht11_handle;

void airQualityInit(void) {
    /* Initialize MQ2 gas sensor */
    mq2Init();

    /* Initialize DHT11 on PA0 */
    DHT11_Init(&dht11_handle, GPIO_PORT_A, 0);
}

int8_t airQualityRead(AirQuality_Data_t *data) {
    int8_t result = 0;

    if (data == NULL) {
        return -1;
    }

    /* Read MQ2 gas sensor ADC */
    data->mq2_adc = mq2ReadADC();

    /* Read DHT11 temperature & humidity */
    DHT11_ReadData(&dht11_handle);
    if (dht11_handle.Status == DHT11_OK) {
        data->temperature = (int8_t)dht11_handle.Temperature;
        data->humidity = (int8_t)dht11_handle.Humidity;
        data->dht11_error = 0;
        result = 0;
    } else {
        /* DHT11 error - keep previous reading but flag error */
        data->temperature = -1;
        data->humidity = -1;
        data->dht11_error = 1;
        result = -1;
    }

    return result;
}

uint16_t airQualityGetMQ2(void) {
    return mq2ReadADC();
}

int8_t airQualityGetTemperature(AirQuality_Data_t *data) {
    if (data == NULL) {
        return -1;
    }
    return data->temperature;
}

int8_t airQualityGetHumidity(AirQuality_Data_t *data) {
    if (data == NULL) {
        return -1;
    }
    return data->humidity;
}
