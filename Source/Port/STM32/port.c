/**
 * @file port.c
 * @brief RTOS Port Implementation for STM32F1 (ARM Cortex-M3)
 *
 * This file provides hardware-specific implementations for the RTOS.
 * Uses driver layer (rcc, gpio, systick) for hardware access.
 */

#include <stdint.h>
#include "port.h"
#include "rcc.h"
#include "gpio.h"
#include "systick.h"
#include "task.h"
#include "timer.h"

/* System tick counter */
volatile uint32_t systemTick = 0;

/**
 * @brief Initialize system hardware
 *
 * Configuration steps:
 * 1. Initialize GPIO pins (PA0, PA1, PA2, PC13)
 * 2. Initialize all LEDs to OFF state (active-low)
 */
void systemInit(void) {
    // Configure GPIO pins as output 50MHz push-pull
    // PA0, PA1, PA2
    gpioInitPin(GPIO_PORT_A, 0, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
    gpioInitPin(GPIO_PORT_A, 1, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
    gpioInitPin(GPIO_PORT_A, 2, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
    // PC13
    gpioInitPin(GPIO_PORT_C, 13, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);

    // Initialize LED state: all OFF (active-high = 0)
    gpioWritePin(GPIO_PORT_C, 13, 0);
    gpioWritePin(GPIO_PORT_A, 0, 0);
    gpioWritePin(GPIO_PORT_A, 1, 0);
    gpioWritePin(GPIO_PORT_A, 2, 0);
}

/**
 * @brief Initialize SysTick timer for 1ms periodic interrupts
 */
void sysTickInit(void) {
    // Calculate ticks per millisecond: 8MHz / 1000 = 8000
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

    // Update LED states based on elapsed time
    ledControlCallback();

    // Update RTOS task scheduler
    taskSchedule();

    // Update all active software timers
    timerTick();
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