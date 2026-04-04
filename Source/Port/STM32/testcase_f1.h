/**
 * @file testcase1.h
 * @brief LED blinking test case header (STM32F1 version)
 *
 * Uses 4 LEDs on STM32F103C8 minimal board:
 * - PC13: LED on board (active-low)
 * - PA0, PA1, PA2: External LEDs (active-high)
 */

#ifndef TESTCASE1_H
#define TESTCASE1_H

#include <stdint.h>

/**
 * @brief Initialize LED control for 4 LEDs with different blink periods
 *
 * LED Timing Configuration:
 * - PC13: 1000ms cycle (500ms ON + 500ms OFF) - FASTEST
 * - PA0:  1500ms cycle (750ms ON + 750ms OFF) - MEDIUM
 * - PA1:  2500ms cycle (1250ms ON + 1250ms OFF) - SLOW
 * - PA2:  3000ms cycle (1500ms ON + 1500ms OFF) - SLOWEST
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