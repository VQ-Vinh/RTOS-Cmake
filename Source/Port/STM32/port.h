/**
 * @file port.h
 * @brief RTOS Port Interface for STM32F1 (ARM Cortex-M3)
 *
 * This file defines the interface between Kernel and Hardware.
 * Kernel calls these functions, Port implementation provides hardware-specific code.
 */

#ifndef PORT_H
#define PORT_H

#include <stdint.h>

/* =============================================================================
 * System Initialization
 * ============================================================================= */

/**
 * @brief Initialize system hardware (clocks, GPIO, etc.)
 */
void systemInit(void);

/* =============================================================================
 * SysTick Timer
 * ============================================================================= */

/**
 * @brief Initialize SysTick for 1ms tick
 */
void sysTickInit(void);

/**
 * @brief Get system tick counter
 * @return Current tick count (milliseconds)
 */
uint32_t getSystemTick(void);

/**
 * @brief Blocking delay in milliseconds
 * @param ms: Number of milliseconds to delay
 */
void delay_ms(uint32_t ms);

/* =============================================================================
 * LED Control
 * ============================================================================= */

/**
 * @brief LED on (PC13)
 */
void ledOn(void);

/**
 * @brief LED off (PC13)
 */
void ledOff(void);

/**
 * @brief LED toggle (PC13)
 */
void ledToggle(void);

/* LED 0 (PA0) */
void led0On(void);
void led0Off(void);
void led0Toggle(void);

/* LED 1 (PA1) */
void led1On(void);
void led1Off(void);
void led1Toggle(void);

/* LED 2 (PA2) */
void led2On(void);
void led2Off(void);
void led2Toggle(void);

/**
 * @brief LED control callback - called every tick
 *        Implemented in App layer
 */
extern void ledControlCallback(void);

/* =============================================================================
 * Context Switching (for Scheduler)
 * ============================================================================= */

/**
 * @brief Trigger context switch from PendSV interrupt
 */
void triggerPendSV(void);

/**
 * @brief Get current stack pointer
 * @return Current PSP value
 */
uint32_t getCurrentPSP(void);

/**
 * @brief Set current stack pointer
 * @param psp: New PSP value
 */
void setCurrentPSP(uint32_t psp);

#endif /* PORT_H */