#include <stdint.h>
#include "task.h"
#include "timer.h"

// STM32F103 register definitions
#define RCC_BASE        0x40021000
#define GPIOA_BASE      0x40010800
#define GPIOC_BASE      0x40011000
#define SysTick_BASE    0xE000E010

// RCC registers
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

// GPIOA registers
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

// GPIOC registers
#define GPIOC_CRH       (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR       (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))
#define GPIOC_IDR       (*(volatile uint32_t *)(GPIOC_BASE + 0x08))

// SysTick registers
#define SysTick_CTRL    (*(volatile uint32_t *)(SysTick_BASE + 0x00))
#define SysTick_LOAD    (*(volatile uint32_t *)(SysTick_BASE + 0x04))
#define SysTick_VAL     (*(volatile uint32_t *)(SysTick_BASE + 0x08))

#define SystemCoreClock 72000000  // 72MHz
#define SysTick_Frequency 1000    // 1ms tick

// Declare LED control callback from main.c
extern void ledControlCallback(void);

volatile uint32_t systemTick = 0;

void systemInit(void) {
    // Enable GPIOA and GPIOC clocks
    RCC_APB2ENR |= (1 << 2);   // IOPAEN
    RCC_APB2ENR |= (1 << 4);   // IOPCEN
    
    // Configure PA0, PA1, PA2 as outputs
    // CRL configuration: 0x3 = Output, 50MHz, Push-pull
    GPIOA_CRL &= ~(0xFFF);     // Clear PA0, PA1, PA2 bits
    GPIOA_CRL |= (0x333);      // Set PA0, PA1, PA2 to output mode
    
    // Configure PC13 as output (LED)
    // CRH configuration for PC13: 0x3 = Output, 50MHz, Push-pull
    GPIOC_CRH &= ~(0xF << 20); // Clear CN13 bits
    GPIOC_CRH |= (0x3 << 20);  // Set CN13 to output mode
    
    // Initialize all LEDs to OFF (high for active-low)
    GPIOA_ODR |= 0x07;  // PA0, PA1, PA2 high
    GPIOC_ODR |= (1 << 13);  // PC13 high
}

void sysTickInit(void) {
    // Configure SysTick for 1ms interrupt
    uint32_t ticksPerMs = SystemCoreClock / 1000;
    
    SysTick_LOAD = ticksPerMs - 1;
    SysTick_VAL = 0;
    SysTick_CTRL = 0x07;  // Enable SysTick, use processor clock, enable interrupt
}

void SysTick_Handler(void) {
    systemTick++;
    
    // Call LED control callback every SysTick (1ms)
    ledControlCallback();
    
    // Update task scheduler
    taskSchedule();
    
    // Update timers
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
