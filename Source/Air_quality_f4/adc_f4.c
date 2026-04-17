/**
 * @file adc_f4.c
 * @brief STM32F4 ADC Driver Implementation for Air Quality Sensor
 */

#include "adc_f4.h"
#include "../../Port/STM32F4/rcc.h"

// =============================================================================
// Base Addresses (STM32F4)
// =============================================================================
#define ADC1_BASE       0x40012000
#define RCC_BASE        0x40023800

// =============================================================================
// ADC Registers (STM32F4)
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JDR1;
    volatile uint32_t JDR2;
    volatile uint32_t JDR3;
    volatile uint32_t JDR4;
    volatile uint32_t ODR;
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t DR;
} ADC_Reg_t;

#define ADC1    ((ADC_Reg_t *)ADC1_BASE)

// =============================================================================
// RCC Register for ADC clock (STM32F4)
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44))
#define RCC_CCIPR       (*(volatile uint32_t *)(RCC_BASE + 0x4C))

#define RCC_ADC1EN      (1 << 8)
#define ADC12EN         (1 << 0)  // ADC12 clock selection

// =============================================================================
// ADC channel definitions
#define ADC_CHANNEL_0   0    // PA0 for MQ2 sensor on STM32F4 Discovery

void adcInit(void) {
    // Enable ADC1 clock
    RCC_APB2ENR |= RCC_ADC1EN;

    // Configure ADC clock: PCLK2 / 4 (assuming 84MHz PCLK2, gives ~21MHz ADC clock)
    // ADCCP bits in CCIPR: 00 = PCLK2, 01 = PCLK2/2, 10 = /4, 11 = /8
    RCC_CCIPR = (RCC_CCIPR & ~(0x3 << 16)) | (0x2 << 16);  // ADC12 clock = PCLK2/4

    // Configure PA0 as analog input
    // Enable GPIOA clock
    volatile uint32_t *RCC_AHB1ENR = (volatile uint32_t *)(RCC_BASE + 0x30);
    *RCC_AHB1ENR |= (1 << 0);  // GPIOA EN

    // PA0 mode: analog (bits 1:0 = 11)
    volatile uint32_t *GPIOA_MODER = (volatile uint32_t *)(0x40020000);
    *GPIOA_MODER |= (0x3 << 0);  // Analog mode for PA0

    // ADC Configuration:
    // CR2: Enable ADC, single conversion mode, right alignment
    ADC1->CR2 = (1 << 0)    // ADON - ADC enable
              | (1 << 11);  // ALIGN - right alignment (12-bit)

    // Sample time: 480 cycles for channel 0 (for sensor stability)
    // SMPR2 bits 2:0 for channel 0 (111 = 480 cycles)
    ADC1->SMPR2 |= (0x7 << 0);

    // Configure regular sequence: 1 conversion on channel 0
    // SQR1: L[3:0] = 0000 (1 conversion)
    ADC1->SQR1 = 0;

    // SQR3: SQ1[4:0] = channel 0
    ADC1->SQR3 = ADC_CHANNEL_0;

    // Calibration
    ADC1->CR2 |= (1 << 30);  // RSTCAL - reset calibration
    while (ADC1->CR2 & (1 << 30));  // Wait for reset
    ADC1->CR2 |= (1 << 31);  // CAL - start calibration
    while (ADC1->CR2 & (1 << 31));  // Wait for calibration
}

uint16_t adcRead(void) {
    // Start conversion
    ADC1->CR2 |= (1 << 30);  // SWSTART - start conversion

    // Wait for conversion complete (EOC flag in SR)
    while (!(ADC1->SR & (1 << 1)));  // Wait for EOC

    // Return data (lower 12 bits)
    return (uint16_t)(ADC1->DR & 0x0FFF);
}
