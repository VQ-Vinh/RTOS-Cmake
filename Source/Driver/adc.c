/**
 * @file adc.c
 * @brief STM32F1 ADC Driver Implementation - Generic ADC peripheral driver
 * Supports multiple channels with configurable sample time
 */

#include "adc.h"
#include "gpio.h"
#include "rcc.h"
#include <stddef.h>

// =============================================================================
// Base Addresses
// =============================================================================
#define ADC1_BASE       0x40012400

// =============================================================================
// ADC Registers
// =============================================================================
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JOFR1;
    volatile uint32_t JOFR2;
    volatile uint32_t JOFR3;
    volatile uint32_t JOFR4;
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t JSQR;
    volatile uint32_t JDR1;
    volatile uint32_t JDR2;
    volatile uint32_t JDR3;
    volatile uint32_t JDR4;
    volatile uint32_t DR;
} ADC_Reg_t;

#define ADC1    ((ADC_Reg_t *)ADC1_BASE)

// =============================================================================
// RCC clock enable
// =============================================================================
#define RCC_ADC1EN      (1 << 9)

// =============================================================================
// Static variables
// =============================================================================
static ADC_Channel_t s_channels[8];  /* Max 8 channels */
static uint8_t s_numChannels = 0;
static uint8_t s_initialized = 0;

// =============================================================================
// Helper functions
// =============================================================================

/**
 * @brief Get GPIO port and pin for ADC channel
 * @return: 1 if valid, 0 if channel not valid for ADC1
 */
static uint8_t adcGetChannelPin(uint8_t channel, GPIO_Port_t *port, uint8_t *pin) {
    /* STM32F1 ADC1 channel mapping */
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
        case 14: *port = GPIO_PORT_C; *pin = 4; return 1;
        case 15: *port = GPIO_PORT_C; *pin = 5; return 1;
        case 16: *port = GPIO_PORT_C; *pin = 0; return 1;  /* Internal temp */
        case 17: *port = GPIO_PORT_C; *pin = 1; return 1;  /* Internal Vref */
        default: return 0;
    }
}

/**
 * @brief Configure GPIO pin for ADC channel
 */
static void adcConfigPin(uint8_t channel) {
    GPIO_Port_t port;
    uint8_t pin;

    if (adcGetChannelPin(channel, &port, &pin)) {
        gpioInitPin(port, pin, GPIO_MODE_INPUT, GPIO_CNF_ANALOG);
    }
}

void adcInit(ADC_Channel_t *channels, uint8_t numChannels) {
    uint8_t i;

    if (channels == NULL || numChannels == 0 || numChannels > 8) {
        return;
    }

    /* Store channel configurations */
    s_numChannels = numChannels;
    for (i = 0; i < numChannels; i++) {
        s_channels[i] = channels[i];
        adcConfigPin(channels[i].channel);
    }

    /* Enable ADC1 clock */
    rccEnableClock(RCC_ADC1EN);

    /* ADC Configuration: Enable ADC, continuous conversion, right alignment */
    ADC1->CR2 = (1 << 0)    /* ADON - ADC enable */
              | (1 << 1)    /* CONT - continuous conversion mode */
              | (1 << 11);  /* ALIGN - right alignment (12-bit) */

    /* Configure sample time for each channel */
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

    /* Configure regular sequence */
    ADC1->SQR1 = (numChannels - 1) << 0;  /* L[3:0] = number of conversions */

    /* Write channel sequence to SQR3 (first 6 channels) */
    uint32_t sqr3 = 0;
    for (i = 0; i < numChannels && i < 6; i++) {
        sqr3 |= channels[i].channel << (i * 5);
    }
    ADC1->SQR3 = sqr3;

    /* Write remaining channels to SQR2 */
    if (numChannels > 6) {
        uint32_t sqr2 = 0;
        for (i = 6; i < numChannels; i++) {
            sqr2 |= channels[i].channel << ((i - 6) * 5);
        }
        ADC1->SQR2 = sqr2;
    }

    /* Start calibration */
    ADC1->CR2 |= (1 << 2);  /* CAL - calibration */
    while (ADC1->CR2 & (1 << 2));  /* Wait for calibration */

    /* Start first conversion */
    ADC1->CR2 |= (1 << 0);  /* ADON - start conversion */

    s_initialized = 1;
}

uint16_t adcReadChannel(uint8_t channel) {
    uint8_t i;

    if (!s_initialized) {
        return 0;
    }

    /* Find channel index in configured channels */
    for (i = 0; i < s_numChannels; i++) {
        if (s_channels[i].channel == channel) {
            break;
        }
    }

    /* If channel not found, return 0 */
    if (i >= s_numChannels) {
        return 0;
    }

    /* Configure single conversion for this channel */
    ADC1->SQR1 = 0;  /* 1 conversion */
    ADC1->SQR3 = channel;  /* SQ1 = channel */

    /* Start conversion */
    ADC1->CR2 |= (1 << 0);  /* ADON */

    /* Wait for EOC */
    while (!(ADC1->SR & (1 << 1)));

    /* Return data */
    return (uint16_t)(ADC1->DR & 0x0FFF);
}

uint16_t adcRead(uint8_t index) {
    if (!s_initialized || index >= s_numChannels) {
        return 0;
    }

    /* Configure single conversion for this channel index */
    ADC1->SQR1 = 0;  /* 1 conversion */
    ADC1->SQR3 = s_channels[index].channel;  /* SQ1 = channel */

    /* Start conversion */
    ADC1->CR2 |= (1 << 0);  /* ADON */

    /* Wait for EOC */
    while (!(ADC1->SR & (1 << 1)));

    /* Return data */
    return (uint16_t)(ADC1->DR & 0x0FFF);
}

uint8_t adcIsInitialized(void) {
    return s_initialized;
}

uint8_t adcGetChannelCount(void) {
    return s_numChannels;
}
