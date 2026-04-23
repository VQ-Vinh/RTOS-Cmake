/**
 * @file rcc.c
 * @brief STM32F1 RCC Driver Implementation
 */

#include "rcc.h"

// =============================================================================
// Base Addresses
// =============================================================================
#define RCC_BASE        0x40021000
#define FLASH_BASE      0x40022000

// =============================================================================
// RCC Registers
// =============================================================================
#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

// =============================================================================
// Flash Control Register (FLASH_ACR)
// =============================================================================
#define FLASH_ACR        (*(volatile uint32_t *)(FLASH_BASE + 0x00))
#define FLASH_ACR_LATENCY_MASK (0x7 << 0)
#define FLASH_ACR_LATENCY_2   (0x2 << 0)

// =============================================================================
// RCC CR bits
// =============================================================================
#define RCC_CR_HSEON    (1 << 16)
#define RCC_CR_HSERDY   (1 << 17)
#define RCC_CR_PLLON    (1 << 24)
#define RCC_CR_PLLRDY   (1 << 25)

// =============================================================================
// RCC CFGR bits
// =============================================================================
#define RCC_CFGR_SW_MASK      (0x3 << 0)
#define RCC_CFGR_SW_PLL       (0x2 << 0)
#define RCC_CFGR_SWS_MASK     (0x3 << 2)
#define RCC_CFGR_PLLSRC       (1 << 16)
#define RCC_CFGR_PLLMUL_MASK  (0xF << 18)
#define RCC_CFGR_PLLMUL_9     (0x7 << 18)
#define RCC_CFGR_HPRE_DIV1    (0x0 << 4)
#define RCC_CFGR_PPRE1_DIV2   (0x4 << 8)
#define RCC_CFGR_PPRE2_DIV1   (0x0 << 11)

void SystemClock_Config(void) {
    // Step 1: Configure Flash latency for 72MHz
    FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | FLASH_ACR_LATENCY_2;

    // Step 2: Enable HSE (High Speed External)
    RCC_CR |= RCC_CR_HSEON;
    while (!(RCC_CR & RCC_CR_HSERDY));

    // Step 3: Configure PLL
    RCC_CFGR = (RCC_CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL_MASK))
             | RCC_CFGR_PLLSRC
             | RCC_CFGR_PLLMUL_9;

    // Step 4: Configure bus clocks
    RCC_CFGR = (RCC_CFGR & ~(0xF << 4 | 0x7 << 8 | 0x7 << 11))
             | RCC_CFGR_HPRE_DIV1
             | RCC_CFGR_PPRE1_DIV2
             | RCC_CFGR_PPRE2_DIV1;

    // Step 5: Enable PLL
    RCC_CR |= RCC_CR_PLLON;
    while (!(RCC_CR & RCC_CR_PLLRDY));

    // Step 6: Select PLL as system clock
    RCC_CFGR = (RCC_CFGR & ~RCC_CFGR_SW_MASK) | RCC_CFGR_SW_PLL;
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != (RCC_CFGR_SW_PLL << 2));
}

void rccEnableClock(uint32_t peripheral) {
    RCC_APB2ENR |= peripheral;
}

void rccDisableClock(uint32_t peripheral) {
    RCC_APB2ENR &= ~peripheral;
}