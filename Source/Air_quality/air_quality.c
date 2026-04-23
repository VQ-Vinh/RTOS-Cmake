/**
 * @file air_quality.c
 * @brief Air Quality Library Implementation
 *
 * Gộp dữ liệu từ MQ2 và DHT11 thành 1 cấu trúc AirQuality_Data_t
 */

#include "air_quality.h"
#include "../Sensor_Lib/mq2.h"
#include "../Sensor_Lib/dht11.h"
#include "../Driver/gpio.h"
#include <stddef.h>

/* ========== Biến static ========== */

/* Handle cho DHT11 (lưu trữ trạng thái DHT11) */
static DHT11_Handle_t dht11_handle;

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo MQ2 và DHT11
 */
void airQualityInit(void) {
    /* Khởi tạo MQ2 gas sensor */
    mq2Init();

    /* Khởi tạo DHT11 trên PA0 */
    DHT11_Init(&dht11_handle, GPIO_PORT_A, 0);
}

/**
 * @brief Đọc tất cả cảm biến
 *
 * Đọc MQ2 ADC trước, sau đó đọc DHT11
 * Nếu DHT11 lỗi, vẫn trả về MQ2 nhưng đánh dấu dht11_error=1
 */
int8_t airQualityRead(AirQuality_Data_t *data) {
    int8_t result = 0;

    if (data == NULL) {
        return -1;
    }

    /* Đọc MQ2 ADC */
    data->mq2_adc = mq2ReadADC();

    /* Đọc DHT11 nhiệt độ và độ ẩm */
    DHT11_ReadData(&dht11_handle);
    if (dht11_handle.Status == DHT11_OK) {
        data->temperature = (int8_t)dht11_handle.Temperature;
        data->humidity = (int8_t)dht11_handle.Humidity;
        data->dht11_error = 0;
        result = 0;
    } else {
        /* DHT11 lỗi - trả về -1 cho nhiệt độ/độ ẩm */
        data->temperature = -1;
        data->humidity = -1;
        data->dht11_error = 1;
        result = -1;
    }

    return result;
}

/**
 * @brief Đọc trực tiếp giá trị MQ2 ADC
 */
uint16_t airQualityGetMQ2(void) {
    return mq2ReadADC();
}

/**
 * @brief Lấy nhiệt độ từ dữ liệu đã đọc
 */
int8_t airQualityGetTemperature(AirQuality_Data_t *data) {
    if (data == NULL) {
        return -1;
    }
    return data->temperature;
}

/**
 * @brief Lấy độ ẩm từ dữ liệu đã đọc
 */
int8_t airQualityGetHumidity(AirQuality_Data_t *data) {
    if (data == NULL) {
        return -1;
    }
    return data->humidity;
}
