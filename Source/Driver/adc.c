/**
 * @file adc.c
 * @brief ADC Driver Implementation
 */

#include "adc.h"
#include "gpio.h"
#include "rcc.h"
#include <stddef.h>

/* ========== Địa chỉ cơ sở ========== */
#define ADC1_BASE       0x40012400

/* ========== Thanh ghi ADC ========== */
typedef struct {
    volatile uint32_t SR;     /* Status register */
    volatile uint32_t CR1;    /* Control 1 */
    volatile uint32_t CR2;    /* Control 2 */
    volatile uint32_t SMPR1;   /* Sample time 1 */
    volatile uint32_t SMPR2;   /* Sample time 2 */
    volatile uint32_t SQR1;   /* Regular sequence 1 */
    volatile uint32_t SQR2;   /* Regular sequence 2 */
    volatile uint32_t SQR3;   /* Regular sequence 3 */
    volatile uint32_t DR;     /* Data register */
} ADC_Reg_t;

#define ADC1    ((ADC_Reg_t *)ADC1_BASE)

/* ========== Clock enable bit ========== */
#define RCC_ADC1EN      (1 << 9)

/* ========== Biến static ========== */
static ADC_Channel_t s_channels[8];  /* Lưu cấu hình channel */
static uint8_t s_numChannels = 0;
static uint8_t s_initialized = 0;

/* ========== Helper functions ========== */

/**
 * @brief Tìm port/pin tương ứng với channel ADC
 * @return: 1 = hợp lệ, 0 = channel không hợp lệ
 */
static uint8_t adcGetChannelPin(uint8_t channel, GPIO_Port_t *port, uint8_t *pin) {
    /* Map channel ADC sang GPIO pin */
    switch (channel) {
        case 0:  *port = GPIO_PORT_A; *pin = 0; return 1;
        case 1:  *port = GPIO_PORT_A; *pin = 1; return 1;
        case 2:  *port = GPIO_PORT_A; *pin = 2; return 1;
        case 3:  *port = GPIO_PORT_A; *pin = 3; return 1;
        case 4:  *port = GPIO_PORT_A; *pin = 4; return 1;
        case 5:  *port = GPIO_PORT_A; *pin = 5; return 1;
        case 6:  *port = GPIO_PORT_A; *pin = 6; return 1;
        case 7:  *port = GPIO_PORT_A; *pin = 7; return 1;
        case 8:  *port = GPIO_PORT_B; *pin = 0; return 1;
        case 9:  *port = GPIO_PORT_B; *pin = 1; return 1;
        case 10: *port = GPIO_PORT_C; *pin = 0; return 1;
        case 11: *port = GPIO_PORT_C; *pin = 1; return 1;
        case 12: *port = GPIO_PORT_C; *pin = 2; return 1;
        case 13: *port = GPIO_PORT_C; *pin = 3; return 1;
        default: return 0;
    }
}

/**
 * @brief Config chân GPIO cho ADC channel
 */
static void adcConfigPin(uint8_t channel) {
    GPIO_Port_t port;
    uint8_t pin;

    if (adcGetChannelPin(channel, &port, &pin)) {
        gpioInitPin(port, pin, GPIO_MODE_INPUT, GPIO_CNF_ANALOG);
    }
}

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo ADC1 với nhiều channel
 *
 * Quy trình:
 * 1. Config GPIO pins cho các channel
 * 2. Enable ADC1 clock
 * 3. Config sample time cho từng channel
 * 4. Config regular sequence (thứ tự conversion)
 * 5. Calibration và start conversion
 */
void adcInit(ADC_Channel_t *channels, uint8_t numChannels) {
    uint8_t i;

    if (channels == NULL || numChannels == 0 || numChannels > 8) {
        return;
    }

    /* Lưu cấu hình channel */
    s_numChannels = numChannels;
    for (i = 0; i < numChannels; i++) {
        s_channels[i] = channels[i];
        adcConfigPin(channels[i].channel);
    }

    /* Enable ADC1 clock */
    rccEnableClock(RCC_ADC1EN);

    /* Enable ADC, continuous conversion, right alignment */
    ADC1->CR2 = (1 << 0) | (1 << 1) | (1 << 11);

    /* Config sample time cho từng channel */
    for (i = 0; i < numChannels; i++) {
        uint8_t ch = channels[i].channel;
        uint32_t smpr_mask = 0xF << (ch * 3);
        uint32_t smpr_value = (uint32_t)channels[i].sampleTime << (ch * 3);

        if (ch < 10) {
            ADC1->SMPR2 = (ADC1->SMPR2 & ~smpr_mask) | smpr_value;
        } else {
            ADC1->SMPR1 = (ADC1->SMPR1 & ~smpr_mask) | smpr_value;
        }
    }

    /* Config số lượng conversion */
    ADC1->SQR1 = (numChannels - 1) << 0;

    /* Ghi sequence vào SQR3 (6 channel đầu) */
    uint32_t sqr3 = 0;
    for (i = 0; i < numChannels && i < 6; i++) {
        sqr3 |= channels[i].channel << (i * 5);
    }
    ADC1->SQR3 = sqr3;

    /* Calibration */
    ADC1->CR2 |= (1 << 2);
    while (ADC1->CR2 & (1 << 2));

    /* Start conversion */
    ADC1->CR2 |= (1 << 0);

    s_initialized = 1;
}

/**
 * @brief Đọc giá trị từ channel cụ thể
 */
uint16_t adcReadChannel(uint8_t channel) {
    uint8_t i;

    if (!s_initialized) return 0;

    /* Tìm index của channel trong cấu hình */
    for (i = 0; i < s_numChannels; i++) {
        if (s_channels[i].channel == channel) break;
    }
    if (i >= s_numChannels) return 0;

    /* Single conversion cho channel này */
    ADC1->SQR1 = 0;
    ADC1->SQR3 = channel;
    ADC1->CR2 |= (1 << 0);
    while (!(ADC1->SR & (1 << 1)));  /* Chờ EOC */

    return (uint16_t)(ADC1->DR & 0x0FFF);
}

/**
 * @brief Đọc ADC theo index đã cấu hình
 */
uint16_t adcRead(uint8_t index) {
    if (!s_initialized || index >= s_numChannels) return 0;

    ADC1->SQR1 = 0;
    ADC1->SQR3 = s_channels[index].channel;
    ADC1->CR2 |= (1 << 0);
    while (!(ADC1->SR & (1 << 1)));

    return (uint16_t)(ADC1->DR & 0x0FFF);
}

uint8_t adcIsInitialized(void) {
    return s_initialized;
}

uint8_t adcGetChannelCount(void) {
    return s_numChannels;
}
