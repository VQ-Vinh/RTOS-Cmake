/**
 * @file systick.h
 * @brief STM32F1 SysTick Driver
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* System clock frequency in Hz - STM32F1 @ 72MHz */
#ifndef SystemCoreClock
#define SystemCoreClock   72000000
#endif

/**
 * @brief System tick counter (milliseconds)
 */
extern volatile uint32_t systemTick;

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

/**
 * @brief Get current system tick (milliseconds)
 * @return Current tick count
 */
uint32_t getSystemTick(void);

/**
 * @brief Blocking delay in milliseconds
 * @param ms: Number of milliseconds to delay
 */
void delay_ms(uint32_t ms);

/**
 * @brief Microsecond delay using SysTick
 * @param us: Number of microseconds to delay
 * @note Blocking delay, uses current SysTick configuration
 */
void systickDelayUs(uint32_t us);

/**
 * @brief Millisecond delay using SysTick
 * @param ms: Number of milliseconds to delay
 * @note Blocking delay
 */
void systickDelayMs(uint32_t ms);

#endif /* SYSTICK_H */