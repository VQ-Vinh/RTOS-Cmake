/**
 * @file rcc.c
 * @brief STM32F4 RCC Driver Implementation
 */

#include "rcc.h"

// =============================================================================
// Base Addresses (STM32F4)
// =============================================================================
#define RCC_BASE        0x40023800

// =============================================================================
// RCC Registers (STM32F4)
// =============================================================================
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))

void rccEnableClock(uint32_t peripheral) {
    RCC_AHB1ENR |= peripheral;
}

void rccDisableClock(uint32_t peripheral) {
    RCC_AHB1ENR &= ~peripheral;
}