/**
 * @file testcase1.c
 * @brief LED blinking test with different time periods (STM32F4 version)
 *
 * Uses 4 built-in LEDs on STM32F4 Discovery:
 * - PD12: Green LED
 * - PD13: Orange LED
 * - PD14: Red LED
 * - PD15: Blue LED
 *
 * Demonstrates 4 LEDs blinking at different frequencies:
 * - PD12: Fast (500ms cycle = 250ms ON / 250ms OFF)
 * - PD13: Medium (1000ms cycle = 500ms ON / 500ms OFF)
 * - PD14: Slow (1500ms cycle = 750ms ON / 750ms OFF)
 * - PD15: Slowest (2500ms cycle = 1250ms ON / 1250ms OFF)
 *
 * Timing is based on 1ms SysTick interrupts
 */

#include "gpio.h"

// =============================================================================
// LED Blink Period Definitions
// =============================================================================
// Period = complete cycle (ON + OFF time)
// Toggle occurs every Period/2 milliseconds
// Example: 500ms period = 250ms ON + 250ms OFF
// =============================================================================
#define LED_PD12_PERIOD 1000   // PD12: 1000ms cycle = 500ms toggle (FASTEST)
#define LED_PD13_PERIOD 1500   // PD13: 1500ms cycle = 750ms toggle
#define LED_PD14_PERIOD 2500   // PD14: 2500ms cycle = 1250ms toggle
#define LED_PD15_PERIOD 3000   // PD15: 3000ms cycle = 1500ms toggle (SLOWEST)

// =============================================================================
// LED Blink Counters
// =============================================================================
// Each counter tracks elapsed milliseconds since last toggle
// Counter increments by 1 every SysTick (1ms)
// When counter reaches Period/2, LED toggles and counter resets
// =============================================================================
static uint32_t counter_PD12 = 0;
static uint32_t counter_PD13 = 0;
static uint32_t counter_PD14 = 0;
static uint32_t counter_PD15 = 0;

/**
 * @brief Initialize testcase1
 *
 * Resets all LED counters to zero.
 * Hardware initialization (GPIO, SysTick) is handled separately in systemInit()
 */
void testcase1_init(void) {
    counter_PD12 = 0;
    counter_PD13 = 0;
    counter_PD14 = 0;
    counter_PD15 = 0;
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
    // PD12 LED: 1000ms cycle (toggle every 500ms) - FASTEST (Green)
    // -----------------------------------------------------------------
    counter_PD12++;
    if (counter_PD12 >= (LED_PD12_PERIOD / 2)) {
        counter_PD12 = 0;
        gpioTogglePin(GPIO_PORT_D, 12);
    }

    // -----------------------------------------------------------------
    // PD13 LED: 1500ms cycle (toggle every 750ms) - MEDIUM (Orange)
    // -----------------------------------------------------------------
    counter_PD13++;
    if (counter_PD13 >= (LED_PD13_PERIOD / 2)) {
        counter_PD13 = 0;
        gpioTogglePin(GPIO_PORT_D, 13);
    }

    // -----------------------------------------------------------------
    // PD14 LED: 2500ms cycle (toggle every 1250ms) - SLOW (Red)
    // -----------------------------------------------------------------
    counter_PD14++;
    if (counter_PD14 >= (LED_PD14_PERIOD / 2)) {
        counter_PD14 = 0;
        gpioTogglePin(GPIO_PORT_D, 14);
    }

    // -----------------------------------------------------------------
    // PD15 LED: 3000ms cycle (toggle every 1500ms) - SLOWEST (Blue)
    // -----------------------------------------------------------------
    counter_PD15++;
    if (counter_PD15 >= (LED_PD15_PERIOD / 2)) {
        counter_PD15 = 0;
        gpioTogglePin(GPIO_PORT_D, 15);
    }
}