/**
 * @file air_quality.h
 * @brief Air Quality Library - Giao diện thống nhất cho MQ2 và DHT11
 *
 * Chức năng: Đọc cảm biến chất lượng không khí
 * Bao gồm: MQ2 (khí gas), DHT11 (nhiệt độ/độ ẩm)
 */

#ifndef AIR_QUALITY_H
#define AIR_QUALITY_H

#include <stdint.h>

/* ========== Cấu trúc dữ liệu ========== */

/**
 * @brief Dữ liệu chất lượng không khí
 */
typedef struct {
    int8_t temperature;     /* Nhiệt độ (°C) - DHT11: 0-50°C */
    int8_t humidity;        /* Độ ẩm (%RH) - DHT11: 20-90% */
    uint16_t mq2_adc;       /* Giá trị ADC MQ2 (0-4095) */
    uint8_t dht11_error;    /* Lỗi DHT11: 0=ok, 1=lỗi */
} AirQuality_Data_t;

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo tất cả cảm biến
 *
 * Thiết bị:
 * - MQ2: PB0 (ADC channel 8)
 * - DHT11: PA0
 */
void airQualityInit(void);

/**
 * @brief Đọc tất cả cảm biến
 * @param data: Con trỏ lưu dữ liệu đọc được
 * @return: 0 = thành công, -1 = lỗi DHT11
 *
 * Đọc:
 * - MQ2 ADC (12-bit, 0-4095)
 * - DHT11 nhiệt độ (°C) và độ ẩm (%RH)
 */
int8_t airQualityRead(AirQuality_Data_t *data);

/**
 * @brief Đọc giá trị ADC thô từ MQ2
 * @return: Giá trị ADC (0-4095)
 */
uint16_t airQualityGetMQ2(void);

/**
 * @brief Lấy nhiệt độ từ DHT11
 * @param data: Con trỏ dữ liệu từ airQualityRead()
 * @return: Nhiệt độ (°C)
 */
int8_t airQualityGetTemperature(AirQuality_Data_t *data);

/**
 * @brief Lấy độ ẩm từ DHT11
 * @param data: Con trỏ dữ liệu từ airQualityRead()
 * @return: Độ ẩm (%RH)
 */
int8_t airQualityGetHumidity(AirQuality_Data_t *data);

#endif /* AIR_QUALITY_H */
