/**
 * @file port.h
 * @brief RTOS Port Interface for STM32F4 (ARM Cortex-M4)
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