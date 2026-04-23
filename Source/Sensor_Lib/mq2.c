/**
 * @file mq2.c
 * @brief MQ2 Gas Sensor Library Implementation
 *
 * Dùng Driver/adc.c để đọc ADC
 * TODO: Implement PPM calculation
 */

#include "mq2.h"
#include "../Driver/adc.h"

/* ========== Biến static ========== */

static uint8_t s_initialized = 0;

/* Cấu hình ADC channel cho MQ2 */
static ADC_Channel_t mq2_adc_channel[] = {
    {MQ2_ADC_CHANNEL, MQ2_ADC_SAMPLE}
};

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo MQ2
 *
 * Gọi adcInit() với channel 8 (PB0)
 */
void mq2Init(void) {
    adcInit(mq2_adc_channel, 1);
    s_initialized = 1;
}

/**
 * @brief Đọc giá trị ADC thô
 */
uint16_t mq2ReadADC(void) {
    if (!s_initialized) {
        return 0;
    }
    return adcRead(0);  /* Channel index 0 = MQ2_ADC_CHANNEL */
}

/**
 * @brief Đọc nồng độ gas (PPM)
 *
 * TODO: Implement PPM calculation
 * MQ2 sensitivity curve cho LPG, propane, hydrogen, methane...
 *
 * Cách tính:
 * 1. Đọc Rs = (Vc - Vout) / Vout * RL
 * 2. Tính ratio = Rs / Ro (Ro = clean air resistance)
 * 3. Áp dụng power curve: PPM = a * (ratio^b)
 */
uint16_t mq2ReadPPM(void) {
    /* Tạm thời return raw ADC value */
    return mq2ReadADC();
}

/**
 * @brief Kiểm tra MQ2 đã khởi tạo
 */
uint8_t mq2IsInitialized(void) {
    return s_initialized;
}
