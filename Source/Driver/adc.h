/**
 * @file adc.h
 * @brief ADC Driver - Analog to Digital Conversion
 *
 * Chức năng: Đọc giá trị analog từ các channel ADC
 * Hỗ trợ: ADC1 với 16 channel (0-15 trên port A/B/C)
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/* ========== Độ phân giải ADC ========== */
#define ADC_RESOLUTION_12BIT   4095   /* 12-bit: 0 - 4095 */
#define ADC_RESOLUTION_10BIT   1023   /* 10-bit: 0 - 1023 */
#define ADC_RESOLUTION_8BIT   255     /* 8-bit: 0 - 255 */

/* ========== Thời gian sample ========== */
typedef enum {
    ADC_SAMPLE_1_5   = 0x0,   /* 1.5 cycles */
    ADC_SAMPLE_7_5   = 0x1,   /* 7.5 cycles */
    ADC_SAMPLE_13_5   = 0x2,  /* 13.5 cycles */
    ADC_SAMPLE_28_5   = 0x3,  /* 28.5 cycles */
    ADC_SAMPLE_41_5   = 0x4,  /* 41.5 cycles */
    ADC_SAMPLE_55_5   = 0x5,  /* 55.5 cycles */
    ADC_SAMPLE_71_5   = 0x6,  /* 71.5 cycles */
    ADC_SAMPLE_239_5   = 0x7   /* 239.5 cycles */
} ADC_SampleTime_t;

/* ========== Cấu hình channel ========== */
typedef struct {
    uint8_t channel;           /* Channel ADC (0-17) */
    ADC_SampleTime_t sampleTime; /* Thời gian sample */
} ADC_Channel_t;

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo ADC1
 * @param channels: Mảng cấu hình các channel
 * @param numChannels: Số lượng channel
 *
 * Ví dụ:
 *   ADC_Channel_t ch[] = {{8, ADC_SAMPLE_71_5}};
 *   adcInit(ch, 1);
 */
void adcInit(ADC_Channel_t *channels, uint8_t numChannels);

/**
 * @brief Đọc ADC channel cụ thể
 * @param channel: Số channel (0-17)
 * @return: Giá trị ADC (0-4095)
 */
uint16_t adcReadChannel(uint8_t channel);

/**
 * @brief Đọc ADC theo index đã cấu hình
 * @param index: Thứ tự channel đã config (0-based)
 * @return: Giá trị ADC (0-4095)
 */
uint16_t adcRead(uint8_t index);

/**
 * @brief Kiểm tra ADC đã khởi tạo chưa
 * @return: 1 = đã init, 0 = chưa
 */
uint8_t adcIsInitialized(void);

/**
 * @brief Số channel đã cấu hình
 * @return: Số lượng channel
 */
uint8_t adcGetChannelCount(void);

#endif /* ADC_H */
