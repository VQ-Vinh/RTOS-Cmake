/**
 * @file testcase1.c
 * @brief LED blinking test with different time periods
 *
 * Demonstrates 4 LEDs blinking at different frequencies:
 * - PC13: Fast (500ms cycle = 250ms ON / 250ms OFF)
 * - PA0:  Medium (1000ms cycle = 500ms ON / 500ms OFF)
 * - PA1:  Slow (1500ms cycle = 750ms ON / 750ms OFF)
 * - PA2:  Slowest (2500ms cycle = 1250ms ON / 1250ms OFF)
 *
 * Timing is based on 1ms SysTick interrupts
 */

#include "testcase1.h"
#include "gpio.h"

// =============================================================================
// LED Blink Period Definitions
// =============================================================================
// Period = complete cycle (ON + OFF time)
// Toggle occurs every Period/2 milliseconds
// Example: 500ms period = 250ms ON + 250ms OFF
// =============================================================================
#define LED_PC13_PERIOD 1000   // PC13: 1000ms cycle = 500ms toggle
#define LED_PA0_PERIOD  1500  // PA0:  1500ms cycle = 750ms toggle
#define LED_PA1_PERIOD  2500  // PA1:  2500ms cycle = 1250ms toggle
#define LED_PA2_PERIOD  3000  // PA2:  3000ms cycle = 1500ms toggle

// =============================================================================
// LED Blink Counters
// =============================================================================
// Each counter tracks elapsed milliseconds since last toggle
// Counter increments by 1 every SysTick (1ms)
// When counter reaches Period/2, LED toggles and counter resets
// =============================================================================
static uint32_t counter_PC13 = 0;
static uint32_t counter_PA0 = 0;
static uint32_t counter_PA1 = 0;
static uint32_t counter_PA2 = 0;

/**
 * @brief Initialize testcase1
 *
 * Resets all LED counters to zero.
 * Hardware initialization (GPIO, SysTick) is handled separately in systemInit()
 */
void testcase1_init(void) {
    counter_PC13 = 0;
    counter_PA0 = 0;
    counter_PA1 = 0;
    counter_PA2 = 0;
}

/**
 * @brief Update LED states - called every 1ms from SysTick
 *
 * Algorithm for each LED:
 * 1. Increment counter by 1 (1ms elapsed)
 * 2. If counter >= Period/2:
 *    - Toggle LED output
 *    - Reset counter to 0
 *
 * @note This function is called from SysTick_Handler via ledControlCallback()
 */
void testcase1_led_update(void) {
    // -----------------------------------------------------------------
    // PC13 LED: 500ms cycle (toggle every 250ms) - FASTEST
    // -----------------------------------------------------------------
    counter_PC13++;
    if (counter_PC13 >= (LED_PC13_PERIOD / 2)) {
        counter_PC13 = 0;
        gpioTogglePin(GPIO_PORT_C, 13);
    }

    // -----------------------------------------------------------------
    // PA0 LED: 1000ms cycle (toggle every 500ms) - MEDIUM
    // -----------------------------------------------------------------
    counter_PA0++;
    if (counter_PA0 >= (LED_PA0_PERIOD / 2)) {
        counter_PA0 = 0;
        gpioTogglePin(GPIO_PORT_A, 0);
    }

    // -----------------------------------------------------------------
    // PA1 LED: 1500ms cycle (toggle every 750ms) - SLOW
    // -----------------------------------------------------------------
    counter_PA1++;
    if (counter_PA1 >= (LED_PA1_PERIOD / 2)) {
        counter_PA1 = 0;
        gpioTogglePin(GPIO_PORT_A, 1);
    }

    // -----------------------------------------------------------------
    // PA2 LED: 2500ms cycle (toggle every 1250ms) - SLOWEST
    // -----------------------------------------------------------------
    counter_PA2++;
    if (counter_PA2 >= (LED_PA2_PERIOD / 2)) {
        counter_PA2 = 0;
        gpioTogglePin(GPIO_PORT_A, 2);
    }
}
