/**
 * @file adc.c
 * @brief STM32F1 ADC Driver Implementation for Air Quality Sensor
 */

#include "adc.h"
#include "../Port/STM32/gpio.h"
#include "../Port/STM32/rcc.h"

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
// ADC channel definitions
// =============================================================================
#define ADC_CHANNEL_8   8    // PB0 for MQ2 sensor

void adcInit(void) {
    // Enable ADC1 clock
    rccEnableClock(RCC_ADC1EN);

    // Configure PB0 as analog input (no pull, no speed)
    // For STM32F1, ADC channel 8 is on PB0
    gpioInitPin(GPIO_PORT_B, 0, GPIO_MODE_INPUT, GPIO_CNF_ANALOG);

    // ADC Configuration:
    // CR2: Enable ADC, enable continuous conversion, right alignment
    ADC1->CR2 = (1 << 0)    // ADON - ADC enable
              | (1 << 1)    // CONT - continuous conversion mode
              | (1 << 11);  // ALIGN - right alignment (12-bit)

    // Sample time: 71.5 cycles for channel 8 (longest, for sensor stability)
    // SMPR2 bits 9-11 for channel 8 (111 = 71.5 cycles)
    ADC1->SMPR2 |= (0x7 << (ADC_CHANNEL_8 * 3));

    // Configure regular sequence: 1 conversion on channel 9
    // SQR1: 0000 0000 0000 0000 0000 0000 0100 0000
    // L[3:0] = 0000 (1 conversion)
    ADC1->SQR1 = 0;

    // SQR3: 0000 0000 0000 0000 0000 0000 0000 1000
    // SQ1[4:0] = 01000 = channel 8
    ADC1->SQR3 = ADC_CHANNEL_8;

    // Start calibration
    ADC1->CR2 |= (1 << 2);  // CAL - calibration
    while (ADC1->CR2 & (1 << 2));  // Wait for calibration to complete
}

uint16_t adcRead(void) {
    // Start conversion
    ADC1->CR2 |= (1 << 0);  // ADON - start conversion

    // Wait for conversion complete (EOC flag in SR)
    while (!(ADC1->SR & (1 << 1)));  // Wait for EOC

    // Return data (lower 12 bits)
    return (uint16_t)(ADC1->DR & 0x0FFF);
}