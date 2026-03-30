/**
 * @file systick.h
 * @brief STM32F1 SysTick Driver
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* System clock frequency in Hz */
#define HSI_CLOCK       8000000

/**
 * @brief Initialize SysTick timer
 * @param ticks: Number of ticks per interrupt
 */
void systickInit(uint32_t ticks);

/**
 * @brief Start SysTick timer
 */
void systickStart(void);

/**
 * @brief Stop SysTick timer
 */
void systickStop(void);

/**
 * @brief Enable SysTick interrupt
 */
void systickEnableInt(void);

/**
 * @brief Disable SysTick interrupt
 */
void systickDisableInt(void);

/**
 * @brief Get current SysTick value
 * @return Current counter value
 */
uint32_t systickGetValue(void);

/**
 * @brief Check if SysTick has counted to zero
 * @return 1 if countflag, 0 otherwise
 */
uint8_t systickGetCountFlag(void);

#endif /* SYSTICK_H */