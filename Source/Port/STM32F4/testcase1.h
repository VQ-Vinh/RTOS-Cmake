/**
 * @file testcase1.h
 * @brief LED blinking test case header (STM32F4 version)
 *
 * Uses 4 built-in LEDs on STM32F4 Discovery:
 * - PD12: Green LED
 * - PD13: Orange LED
 * - PD14: Red LED
 * - PD15: Blue LED
 */

#ifndef TESTCASE1_H
#define TESTCASE1_H

#include <stdint.h>

/**
 * @brief Initialize LED control for 4 LEDs with different blink periods
 *
 * LED Timing Configuration:
 * - PD12: 1000ms cycle (500ms ON + 500ms OFF) - FASTEST
 * - PD13: 1500ms cycle (750ms ON + 750ms OFF) - MEDIUM
 * - PD14: 2500ms cycle (1250ms ON + 1250ms OFF) - SLOW
 * - PD15: 3000ms cycle (1500ms ON + 1500ms OFF) - SLOWEST
 */
void testcase1_init(void);

/**
 * @brief LED control callback - called from SysTick_Handler every 1ms
 *
 * Manages blinking logic for all 4 LEDs by comparing elapsed time
 * with predefined periods. Uses counter-based approach:
 * counter increments every 1ms, when counter >= period/2, LED toggles.
 */
void testcase1_led_update(void);

#endif // TESTCASE1_H