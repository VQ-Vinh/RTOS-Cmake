/**
 * @file port.c
 * @brief STM32F103 hardware abstraction layer
 *
 * Provides low-level peripheral access for:
 * - GPIO (PA0, PA1, PA2, PC13)
 * - SysTick timer (1ms system tick)
 * - System clock configuration
 */

#include <stdint.h>
#include "task.h"
#include "timer.h"

// =============================================================================
// Base Addresses
// =============================================================================
#define RCC_BASE        0x40021000  // Reset & Clock Control
#define GPIOA_BASE      0x40010800  // GPIO Port A
#define GPIOC_BASE      0x40011000  // GPIO Port C
#define SysTick_BASE    0xE000E010  // System Timer

// =============================================================================
// RCC Registers (Reset & Clock Control)
// =============================================================================
#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00))  // Control Register
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x04))  // Clock Configuration
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))  // APB2 Peripheral Clock Enable

// =============================================================================
// GPIOA Registers (Port A)
// =============================================================================
// CRL: Control register low (pins 0-7)
// ODR: Output data register
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

// =============================================================================
// GPIOC Registers (Port C)
// =============================================================================
// CRH: Control register high (pins 8-15)
// ODR: Output data register
// IDR: Input data register
#define GPIOC_CRH       (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR       (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))
#define GPIOC_IDR       (*(volatile uint32_t *)(GPIOC_BASE + 0x08))

// =============================================================================
// SysTick Registers (System Timer)
// =============================================================================
// CTRL: Control & Status register
// LOAD: Reload value register
// VAL: Current value register
#define SysTick_CTRL    (*(volatile uint32_t *)(SysTick_BASE + 0x00))
#define SysTick_LOAD    (*(volatile uint32_t *)(SysTick_BASE + 0x04))
#define SysTick_VAL     (*(volatile uint32_t *)(SysTick_BASE + 0x08))

// =============================================================================
// System Clock Configuration
// =============================================================================
// Using HSI (Internal High-Speed Oscillator) at 8MHz
// Note: PLL configuration not included - would require external crystal for 72MHz
#define SystemCoreClock 8000000   // 8MHz HSI clock
#define SysTick_Frequency 1000    // Target: 1ms (1000Hz) tick

// Declare LED control callback from main.c
extern void ledControlCallback(void);

volatile uint32_t systemTick = 0;

/**
 * @brief Initialize system hardware
 *
 * Configuration steps:
 * 1. Enable clock for GPIOA and GPIOC peripherals
 * 2. Configure PA0, PA1, PA2 as push-pull output (50MHz)
 * 3. Configure PC13 as push-pull output (50MHz)
 * 4. Initialize all LEDs to OFF state (active-low LEDs)
 *
 * Note: System runs on HSI (8MHz) - no PLL
 */
void systemInit(void) {
    // Enable GPIOA and GPIOC peripheral clocks
    // Bit 2: IOPAEN (GPIOA clock enable)
    // Bit 4: IOPCEN (GPIOC clock enable)
    RCC_APB2ENR |= (1 << 2);
    RCC_APB2ENR |= (1 << 4);

    // Configure PA0, PA1, PA2 as general purpose output
    // Each pin needs 4 bits: CNF(2 bits) + MODE(2 bits)
    // 0x3 in MODE = 50MHz, 0x0 in CNF = push-pull
    // Format: [CNF1:MODE1][CNF0:MODE0] for each pin
    GPIOA_CRL &= ~(0xFFF);     // Clear bits for PA0, PA1, PA2
    GPIOA_CRL |= (0x333);      // Set: MODE=50MHz, CNF=push-pull

    // Configure PC13 as general purpose output (on GPIOC port)
    // PC13 is controlled by bits 20-23 in CRH (pin 13)
    // Same configuration: 50MHz push-pull
    GPIOC_CRH &= ~(0xF << 20); // Clear CNF/MODE bits for PC13
    GPIOC_CRH |= (0x3 << 20);  // Set MODE=50MHz, CNF=push-pull

    // Initialize LED state: all OFF (active-low means high=off)
    // PA0, PA1, PA2: set bits high to turn LEDs off
    GPIOA_ODR |= 0x07;
    // PC13: set bit high to turn LED off
    GPIOC_ODR |= (1 << 13);
}

/**
 * @brief Initialize SysTick timer for 1ms periodic interrupts
 *
 * SysTick configuration:
 * - Source: processor clock (HSI = 8MHz)
 * - Reload: 8000 - 1 = 7999 ticks = 1ms at 8MHz
 * - Enable: counter enabled, interrupt enabled
 *
 * CTRL bits: [RESERVED][RESERVED][RESERVED][COUNTFLAG][RESERVED][RESERVED][RESERVED][CLK_SRC][RESERVED][RESERVED][RESERVED][RESERVED][RESERVED][RESERVED][IE][ENABLE]
 *            = 0x07 = 0000 0111b = ENABLE(1) + CLK_SRC(1) = 8MHz source + IE(1) = interrupt enabled
 */
void sysTickInit(void) {
    // Calculate ticks per millisecond: 8MHz / 1000 = 8000
    uint32_t ticksPerMs = SystemCoreClock / 1000;

    // LOAD: value loaded into VAL when counter reaches 0
    SysTick_LOAD = ticksPerMs - 1;
    // VAL: clear current value to start from reload value
    SysTick_VAL = 0;
    // CTRL: enable timer, use processor clock, enable interrupt
    SysTick_CTRL = 0x07;
}

/**
 * @brief SysTick interrupt handler - called every 1ms
 *
 * This is the system tick interrupt that provides:
 * 1. Global millisecond counter (systemTick)
 * 2. LED update callback (called every 1ms)
 * 3. Task scheduler update (for RTOS)
 * 4. Timer tick update (for software timers)
 */
void SysTick_Handler(void) {
    // Increment global system tick counter
    systemTick++;

    // Update LED states based on elapsed time
    ledControlCallback();

    // Update RTOS task scheduler
    // Checks blocked tasks and moves ready tasks to execution queue
    taskSchedule();

    // Update all active software timers
    timerTick();
}

void ledOn(void) {
    GPIOC_ODR &= ~(1 << 13);  // Set PC13 low (LED on - active low)
}

void ledOff(void) {
    GPIOC_ODR |= (1 << 13);   // Set PC13 high (LED off - active low)
}

void ledToggle(void) {
    GPIOC_ODR ^= (1 << 13);   // Toggle PC13
}

// PA0 LED functions (active low)
void led0On(void) {
    GPIOA_ODR &= ~(1 << 0);  // Set PA0 low
}

void led0Off(void) {
    GPIOA_ODR |= (1 << 0);   // Set PA0 high
}

void led0Toggle(void) {
    GPIOA_ODR ^= (1 << 0);   // Toggle PA0
}

// PA1 LED functions (active low)
void led1On(void) {
    GPIOA_ODR &= ~(1 << 1);  // Set PA1 low
}

void led1Off(void) {
    GPIOA_ODR |= (1 << 1);   // Set PA1 high
}

void led1Toggle(void) {
    GPIOA_ODR ^= (1 << 1);   // Toggle PA1
}

// PA2 LED functions (active low)
void led2On(void) {
    GPIOA_ODR &= ~(1 << 2);  // Set PA2 low
}

void led2Off(void) {
    GPIOA_ODR |= (1 << 2);   // Set PA2 high
}

void led2Toggle(void) {
    GPIOA_ODR ^= (1 << 2);   // Toggle PA2
}

void delay_ms(uint32_t ms) {
    uint32_t start = systemTick;
    while ((systemTick - start) < ms);
}

uint32_t getSystemTick(void) {
    return systemTick;
}
