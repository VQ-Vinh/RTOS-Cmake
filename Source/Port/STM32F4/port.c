/**
 * @file port.c
 * @brief RTOS Port Implementation for STM32F4 (ARM Cortex-M4)
 *
 * This file provides hardware-specific implementations for the RTOS.
 * Uses driver layer (rcc, gpio, systick) for hardware access.
 */

#include <stdint.h>
#include "port.h"
#include "rcc.h"
#include "gpio.h"
#include "systick.h"

/* System tick counter */
volatile uint32_t systemTick = 0;

/**
 * @brief Initialize system hardware
 *
 * Configuration steps:
 * 1. Initialize GPIO pins (PD12, PD13, PD14, PD15) for 4 built-in LEDs
 * 2. Initialize all LEDs to OFF state
 */
void systemInit(void) {
    // Configure GPIO pins as output using STM32F4 API
    // PD12, PD13, PD14, PD15 (built-in LEDs on STM32F4 Discovery)
    gpioInitPinSimple(GPIO_PORT_D, 12, GPIO_MODE_OUTPUT);
    gpioInitPinSimple(GPIO_PORT_D, 13, GPIO_MODE_OUTPUT);
    gpioInitPinSimple(GPIO_PORT_D, 14, GPIO_MODE_OUTPUT);
    gpioInitPinSimple(GPIO_PORT_D, 15, GPIO_MODE_OUTPUT);

    // Initialize LED state: all OFF
    gpioWritePin(GPIO_PORT_D, 12, 0);
    gpioWritePin(GPIO_PORT_D, 13, 0);
    gpioWritePin(GPIO_PORT_D, 14, 0);
    gpioWritePin(GPIO_PORT_D, 15, 0);
}

/**
 * @brief Initialize SysTick timer for 1ms periodic interrupts
 */
void sysTickInit(void) {
    // Calculate ticks per millisecond: 16MHz / 1000 = 16000
    uint32_t ticksPerMs = HSI_CLOCK / 1000;
    systickInit(ticksPerMs);
}

/**
 * @brief SysTick interrupt handler - called every 1ms
 *
 * This is the system tick interrupt that provides:
 * 1. Global millisecond counter (systemTick)
 * 2. LED update callback
 * 3. Task scheduler update
 * 4. Timer tick update
 */
void SysTick_Handler(void) {
    // Increment global system tick counter
    systemTick++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = systemTick;
    while ((systemTick - start) < ms);
}

uint32_t getSystemTick(void) {
    return systemTick;
}

/* Context switching functions */
void triggerPendSV(void) {
    // Trigger PendSV interrupt for context switch
    volatile uint32_t *SHPR3 = (volatile uint32_t *)0xE000ED20;
    *SHPR3 |= (0xFF << 16);  // Set PendSV priority
    volatile uint32_t *ICSR = (volatile uint32_t *)0xE000ED04;
    *ICSR = (1 << 28);       // Trigger PendSV
}

uint32_t getCurrentPSP(void) {
    uint32_t result;
    __asm volatile ("mrs %0, psp" : "=r" (result));
    return result;
}

void setCurrentPSP(uint32_t psp) {
    __asm volatile ("msr psp, %0" : : "r" (psp));
}