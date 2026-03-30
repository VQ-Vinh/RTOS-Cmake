/**
 * @file systick.c
 * @brief STM32F1 SysTick Driver Implementation
 */

#include "systick.h"

// =============================================================================
// SysTick Registers
// =============================================================================
#define SysTick_BASE    0xE000E010

#define SysTick_CTRL    (*(volatile uint32_t *)(SysTick_BASE + 0x00))
#define SysTick_LOAD    (*(volatile uint32_t *)(SysTick_BASE + 0x04))
#define SysTick_VAL     (*(volatile uint32_t *)(SysTick_BASE + 0x08))

// CTRL bits
#define SYSTICK_CTRL_ENABLE     (1 << 0)
#define SYSTICK_CTRL_TICKINT   (1 << 1)
#define SYSTICK_CTRL_CLKSOURCE (1 << 2)
#define SYSTICK_CTRL_COUNTFLAG (1 << 16)

void systickInit(uint32_t ticks) {
    // Set reload value
    SysTick_LOAD = ticks - 1;
    // Clear current value
    SysTick_VAL = 0;
    // Configure: enable, processor clock, interrupt enabled
    SysTick_CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

void systickStart(void) {
    SysTick_CTRL |= SYSTICK_CTRL_ENABLE;
}

void systickStop(void) {
    SysTick_CTRL &= ~SYSTICK_CTRL_ENABLE;
}

void systickEnableInt(void) {
    SysTick_CTRL |= SYSTICK_CTRL_TICKINT;
}

void systickDisableInt(void) {
    SysTick_CTRL &= ~SYSTICK_CTRL_TICKINT;
}

uint32_t systickGetValue(void) {
    return SysTick_VAL;
}

uint8_t systickGetCountFlag(void) {
    return (SysTick_CTRL >> 16) & 0x1;
}