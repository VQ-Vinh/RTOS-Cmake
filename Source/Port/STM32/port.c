/**
 * @file port.c
 * @brief STM32F1 Hardware Port - Minimal version for ADC + UART
 */

#include <stdint.h>
#include "gpio.h"

// =============================================================================
// RCC Registers (Reset and Clock Control)
// =============================================================================
#define RCC_BASE        0x40021000
#define RCC_CR         (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x1C))

/* RCC CR bits */
#define RCC_CR_HSEON   (1 << 16)
#define RCC_CR_HSERDY  (1 << 17)
#define RCC_CR_PLLON   (1 << 24)
#define RCC_CR_PLLRDY  (1 << 25)

/* RCC CFGR bits */
#define RCC_CFGR_SW_MASK    (0x3 << 0)
#define RCC_CFGR_SW_PLL     (0x2 << 0)
#define RCC_CFGR_SWS_MASK   (0x3 << 2)
#define RCC_CFGR_PLLSRC     (1 << 16)
#define RCC_CFGR_PLLMUL_MASK (0xF << 18)
#define RCC_CFGR_PLLMUL_9   (0x7 << 18)  // 8MHz * 9 = 72MHz
#define RCC_CFGR_HPRE_DIV1  (0x0 << 4)   // HCLK = SYSCLK / 1
#define RCC_CFGR_PPRE1_DIV2 (0x4 << 8)   // APB1 = HCLK / 2 (36MHz)
#define RCC_CFGR_PPRE2_DIV1 (0x0 << 11)  // APB2 = HCLK / 1 (72MHz)

/* RCC APB2ENR bits */
#define RCC_IOPAEN     (1 << 2)
#define RCC_IOPBEN     (1 << 3)
#define RCC_IOPCEN     (1 << 4)
#define RCC_ADC1EN     (1 << 9)
#define RCC_USART1EN   (1 << 14)

// =============================================================================
// Flash Control Register (FLASH_ACR)
// =============================================================================
#define FLASH_BASE     0x40022000
#define FLASH_ACR      (*(volatile uint32_t *)(FLASH_BASE + 0x00))
#define FLASH_ACR_LATENCY_MASK (0x7 << 0)
#define FLASH_ACR_LATENCY_2    (0x2 << 0)  // 2 wait states for 72MHz

// System tick counter (milliseconds)
volatile uint32_t systemTick = 0;

// =============================================================================
// SysTick Registers
// =============================================================================
#define SysTick_BASE    0xE000E010
#define SysTick_CTRL    (*(volatile uint32_t *)(SysTick_BASE + 0x00))
#define SysTick_LOAD    (*(volatile uint32_t *)(SysTick_BASE + 0x04))
#define SysTick_VAL     (*(volatile uint32_t *)(SysTick_BASE + 0x08))

#define SYSTICK_CTRL_ENABLE     (1 << 0)
#define SYSTICK_CTRL_TICKINT   (1 << 1)
#define SYSTICK_CTRL_CLKSOURCE (1 << 2)
#define SYSTICK_CTRL_COUNTFLAG (1 << 16)

/**
 * @brief Configure System Clock to 72MHz from external 8MHz crystal
 * Equivalent to STM32CubeMX SystemClock_Config()
 * 
 * Configuration:
 * - HSE (High Speed External): 8MHz crystal
 * - PLL: Enabled, source HSE, multiplier = 9
 * - SYSCLK: 72MHz (8MHz * 9)
 * - HCLK: 72MHz (AHB divider = 1)
 * - PCLK1: 36MHz (APB1 divider = 2)
 * - PCLK2: 72MHz (APB2 divider = 1)
 * - Flash latency: 2 wait states
 */
void SystemClock_Config(void) {
    // Step 1: Configure Flash latency for 72MHz
    // Clear LATENCY bits and set to 2 wait states
    FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | FLASH_ACR_LATENCY_2;
    
    // Step 2: Enable HSE (High Speed External)
    RCC_CR |= RCC_CR_HSEON;
    
    // Wait for HSE to be ready
    while (!(RCC_CR & RCC_CR_HSERDY));
    
    // Step 3: Configure PLL
    // - Source: HSE (bit 16 = 1)
    // - Multiplier: 9 (bits 18-21 = 0111)
    RCC_CFGR = (RCC_CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL_MASK)) 
             | RCC_CFGR_PLLSRC 
             | RCC_CFGR_PLLMUL_9;
    
    // Step 4: Configure bus clocks (AHB, APB1, APB2)
    RCC_CFGR = (RCC_CFGR & ~(0xF << 4 | 0x7 << 8 | 0x7 << 11))
             | RCC_CFGR_HPRE_DIV1    // HCLK = SYSCLK / 1
             | RCC_CFGR_PPRE1_DIV2   // APB1 = HCLK / 2
             | RCC_CFGR_PPRE2_DIV1;  // APB2 = HCLK / 1
    
    // Step 5: Enable PLL
    RCC_CR |= RCC_CR_PLLON;
    
    // Wait for PLL to be ready
    while (!(RCC_CR & RCC_CR_PLLRDY));
    
    // Step 6: Select PLL as system clock source
    RCC_CFGR = (RCC_CFGR & ~RCC_CFGR_SW_MASK) | RCC_CFGR_SW_PLL;
    
    // Wait for PLL to be used as system clock
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != (RCC_CFGR_SW_PLL << 2));
}

/**
 * @brief Initialize system hardware (RCC + GPIO clocks)
 */
void systemInit(void) {
    // Configure system clock to 72MHz
    SystemClock_Config();
    
    // Enable GPIO clocks and ADC
    RCC_APB2ENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_USART1EN;
}

/**
 * @brief Initialize SysTick timer for 1ms periodic interrupts
 * Configured for 72MHz system clock
 */
void sysTickInit(void) {
    // SysTick reload value: 72MHz / 1000 = 72000 clock cycles per 1ms
    // Load value = reload_value - 1
    SysTick_LOAD = 72000 - 1;
    SysTick_VAL = 0;
    SysTick_CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

/**
 * @brief SysTick interrupt handler - called every 1ms
 */
void SysTick_Handler(void) {
    systemTick++;
}

/**
 * @brief Delay milliseconds
 */
void delay_ms(uint32_t ms) {
    uint32_t start = systemTick;
    while ((systemTick - start) < ms);
}

/**
 * @brief Get current system tick
 */
uint32_t getSystemTick(void) {
    return systemTick;
}