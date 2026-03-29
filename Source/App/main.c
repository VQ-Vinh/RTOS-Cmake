#include <stdint.h>
#include <stddef.h>
#include "task.h"
#include "timer.h"
#include "semaphore.h"

// Port functions
extern void systemInit(void);
extern void sysTickInit(void);
extern void ledOn(void);
extern void ledOff(void);
extern void ledToggle(void);
extern void led0On(void);
extern void led0Off(void);
extern void led0Toggle(void);
extern void led1On(void);
extern void led1Off(void);
extern void led1Toggle(void);
extern void led2On(void);
extern void led2Off(void);
extern void led2Toggle(void);
extern void delay_ms(uint32_t ms);
extern uint32_t getSystemTick(void);

// LED counters for each LED with different periods
static volatile uint32_t ledPC13Counter = 0;  // 250ms toggle = 500ms cycle
static volatile uint32_t ledPA0Counter = 0;   // 100ms toggle = 200ms cycle (faster)
static volatile uint32_t ledPA1Counter = 0;   // 333ms toggle ≈ 666ms cycle (medium)
static volatile uint32_t ledPA2Counter = 0;   // 500ms toggle = 1000ms cycle (slower)

// LED control callbacks for each LED
// LED PC13: 250ms ON, 250ms OFF (500ms total cycle)
void led_pc13_control(void) {
    ledPC13Counter++;
    if (ledPC13Counter >= 250) {
        ledPC13Counter = 0;
        ledToggle();
    }
}

// LED PA0: 100ms ON, 100ms OFF (200ms total cycle) - FAST
void led_pa0_control(void) {
    ledPA0Counter++;
    if (ledPA0Counter >= 100) {
        ledPA0Counter = 0;
        led0Toggle();
    }
}

// LED PA1: 333ms ON, 333ms OFF (666ms total cycle) - MEDIUM
void led_pa1_control(void) {
    ledPA1Counter++;
    if (ledPA1Counter >= 333) {
        ledPA1Counter = 0;
        led1Toggle();
    }
}

// LED PA2: 500ms ON, 500ms OFF (1000ms total cycle) - SLOW
void led_pa2_control(void) {
    ledPA2Counter++;
    if (ledPA2Counter >= 500) {
        ledPA2Counter = 0;
        led2Toggle();
    }
}

// Call all LED controls from main SysTick handler
void ledControlCallback(void) {
    led_pc13_control();
    led_pa0_control();
    led_pa1_control();
    led_pa2_control();
}

int main(void) {
    // Initialize system
    systemInit();
    sysTickInit();
    
    // Initialize RTOS
    taskInit();
    timerInit();
    
    // Initialize scheduler
    taskSchedule();
    
    // Main loop - LED control happens via ledControlCallback() called from SysTick
    // LED Blynk patterns:
    // - PC13 (LED on module): 250ms toggle (500ms full cycle)
    // - PA0: 100ms toggle (200ms full cycle) - FASTEST
    // - PA1: 333ms toggle (666ms full cycle) - MEDIUM
    // - PA2: 500ms toggle (1000ms full cycle) - SLOWEST
    while(1) {
        // Get current task if needed
        Task *current = taskGetCurrent();
        
        // Do minimal work - let SysTick handle LED scheduling
        if (current != NULL && current->function != NULL) {
            current->function();
        }
        
        // Yield to allow scheduler to work
        taskYield();
    }
    
    return 0;
}
