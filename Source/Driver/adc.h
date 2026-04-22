/**
 * @file adc.h
 * @brief STM32F1 ADC Driver - Generic ADC peripheral driver
 * Supports multiple channels configuration
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/* ADC Resolution */
#define ADC_RESOLUTION_12BIT   4095
#define ADC_RESOLUTION_10BIT   1023
#define ADC_RESOLUTION_8BIT   255

/* ADC Sample Time (cycles) */
typedef enum {
    ADC_SAMPLE_1_5   = 0x0,   /* 1.5 cycles */
    ADC_SAMPLE_7_5   = 0x1,   /* 7.5 cycles */
    ADC_SAMPLE_13_5   = 0x2,   /* 13.5 cycles */
    ADC_SAMPLE_28_5   = 0x3,   /* 28.5 cycles */
    ADC_SAMPLE_41_5   = 0x4,   /* 41.5 cycles */
    ADC_SAMPLE_55_5   = 0x5,   /* 55.5 cycles */
    ADC_SAMPLE_71_5   = 0x6,   /* 71.5 cycles */
    ADC_SAMPLE_239_5   = 0x7    /* 239.5 cycles */
} ADC_SampleTime_t;

/* ADC Channel configuration */
typedef struct {
    uint8_t channel;           /* ADC channel number (0-17) */
    ADC_SampleTime_t sampleTime; /* Sample time */
} ADC_Channel_t;

/**
 * @brief Initialize ADC1 peripheral
 * @param channels: Array of channel configurations
 * @param numChannels: Number of channels in array
 *
 * Example:
 *   ADC_Channel_t ch[] = {{8, ADC_SAMPLE_71_5}};
 *   adcInit(ch, 1);
 */
void adcInit(ADC_Channel_t *channels, uint8_t numChannels);

/**
 * @brief Read ADC value from specific channel
 * @param channel: ADC channel number (0-17)
 * @return: ADC value (0-4095 for 12-bit)
 *
 * Note: Must call adcInit() first
 */
uint16_t adcReadChannel(uint8_t channel);

/**
 * @brief Read ADC value from configured channel by index
 * @param index: Index of configured channel (0-based)
 * @return: ADC value (0-4095 for 12-bit)
 *
 * Note: Must call adcInit() first with channel configuration
 */
uint16_t adcRead(uint8_t index);

/**
 * @brief Check if ADC is initialized
 * @return: 1 if initialized, 0 if not
 */
uint8_t adcIsInitialized(void);

/**
 * @brief Get number of configured channels
 * @return: Number of channels
 */
uint8_t adcGetChannelCount(void);

#endif /* ADC_H */
