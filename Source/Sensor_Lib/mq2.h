/**
 * @file mq2.h
 * @brief MQ2 Gas Sensor Library
 *
 * Chức năng: Đọc cảm biến khí gas MQ2
 * Giao tiếp: ADC channel 8 (PB0)
 */

#ifndef MQ2_H
#define MQ2_H

#include <stdint.h>

/* ========== Cấu hình MQ2 ========== */
#define MQ2_ADC_CHANNEL   8      /* PB0 trên STM32F1 */
#define MQ2_ADC_SAMPLE    ADC_SAMPLE_71_5  /* 71.5 cycles sample time */

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo MQ2 sensor
 *
 * Cấu hình ADC channel 8 (PB0) cho MQ2
 */
void mq2Init(void);

/**
 * @brief Đọc giá trị ADC thô từ MQ2
 * @return: Giá trị ADC (0-4095)
 *
 * Ý nghĩa:
 * - 0 = 0V (không có gas)
 * - 4095 = 3.3V (nồng độ gas tối đa)
 */
uint16_t mq2ReadADC(void);

/**
 * @brief Đọc nồng độ gas (PPM)
 * @return: Giá trị PPM
 *
 * TODO: Chuyển đổi ADC sang PPM dùng sensitivity curve
 * Công thức: PPM = a * (ratio^b)
 */
uint16_t mq2ReadPPM(void);

/**
 * @brief Kiểm tra MQ2 đã khởi tạo chưa
 * @return: 1 = đã init, 0 = chưa
 */
uint8_t mq2IsInitialized(void);

#endif /* MQ2_H */
