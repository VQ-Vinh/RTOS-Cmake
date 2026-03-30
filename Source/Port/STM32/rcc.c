/**
 * @file rcc.c
 * @brief STM32F1 RCC Driver Implementation
 */

#include "rcc.h"

// =============================================================================
// Base Addresses
// =============================================================================
#define RCC_BASE        0x40021000

// =============================================================================
// RCC Registers
// =============================================================================
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

void rccEnableClock(uint32_t peripheral) {
    RCC_APB2ENR |= peripheral;
}

void rccDisableClock(uint32_t peripheral) {
    RCC_APB2ENR &= ~peripheral;
}