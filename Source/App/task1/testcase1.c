#include "testcase1.h"

// LED control functions (defined in port.c)
extern void ledToggle(void);
extern void led0Toggle(void);
extern void led1Toggle(void);
extern void led2Toggle(void);

/**
 * LED Blink Periods Definition
 * Period = ON_time + OFF_time
 * Toggle happens every Period/2 milliseconds
 */
#define LED_PC13_PERIOD 200   // 200ms cycle: 100ms ON + 100ms OFF
#define LED_PA0_PERIOD  500   // 500ms cycle: 250ms ON + 250ms OFF (FASTEST)
#define LED_PA1_PERIOD  1000  // 1000ms cycle: 500ms ON + 500ms OFF (MEDIUM)
#define LED_PA2_PERIOD  1500  // 1500ms cycle: 750ms ON + 750ms OFF (SLOWEST)

/**
 * LED Blink Counters
 * Each counter increments by 1 for each millisecond (from SysTick)
 * When counter >= Period/2, LED toggles and counter resets
 */
static uint32_t counter_PC13 = 0;
static uint32_t counter_PA0 = 0;
static uint32_t counter_PA1 = 0;
static uint32_t counter_PA2 = 0;

/**
 * Initialize testcase1 - Currently no special initialization needed
 */
void testcase1_init(void) {
    // All initializations are done in systemInit() and sysTickInit()
    // Reset all counters to 0
    counter_PC13 = 0;
    counter_PA0 = 0;
    counter_PA1 = 0;
    counter_PA2 = 0;
}

/**
 * Update LED blinking - Called every 1ms from SysTick_Handler
 * 
 * Logic for each LED:
 * - Increment counter by 1 (representing 1ms elapsed)
 * - Compare with Period/2
 * - If >= Period/2: toggle LED and reset counter
 */
void testcase1_led_update(void) {
    // ===== PC13 LED (500ms period) =====
    // Toggle every 250ms (500ms / 2)
    counter_PC13++;
    if (counter_PC13 >= (LED_PC13_PERIOD / 2)) {
        counter_PC13 = 0;
        ledToggle();
    }
    
    // ===== PA0 LED - FAST (200ms period) =====
    // Toggle every 100ms (200ms / 2)
    counter_PA0++;
    if (counter_PA0 >= (LED_PA0_PERIOD / 2)) {
        counter_PA0 = 0;
        led0Toggle();
    }
    
    // ===== PA1 LED - MEDIUM (666ms period) =====
    // Toggle every 333ms (666ms / 2)
    counter_PA1++;
    if (counter_PA1 >= (LED_PA1_PERIOD / 2)) {
        counter_PA1 = 0;
        led1Toggle();
    }
    
    // ===== PA2 LED - SLOW (1000ms period) =====
    // Toggle every 500ms (1000ms / 2)
    counter_PA2++;
    if (counter_PA2 >= (LED_PA2_PERIOD / 2)) {
        counter_PA2 = 0;
        led2Toggle();
    }
}
