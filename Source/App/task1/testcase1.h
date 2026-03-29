#ifndef TESTCASE1_H
#define TESTCASE1_H

#include <stdint.h>

/**
 * @brief Initialize LED control for 3 LEDs with different blink periods
 * 
 * LEDs:
 * - PC13: 500ms cycle (250ms on + 250ms off)
 * - PA0:  200ms cycle (100ms on + 100ms off) - FAST
 * - PA1:  666ms cycle (333ms on + 333ms off) - MEDIUM
 * - PA2:  1000ms cycle (500ms on + 500ms off) - SLOW
 */
void testcase1_init(void);

/**
 * @brief LED control callback - should be called from SysTick_Handler every 1ms
 * 
 * This function manages the blinking logic for all 3 LEDs
 * by comparing elapsed time with defined periods
 */
void testcase1_led_update(void);

#endif // TESTCASE1_H
