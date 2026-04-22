/**
 * @file port.c
 * @brief RTOS Port Implementation for STM32F4 (ARM Cortex-M4)
 *
 * This file provides hardware-specific implementations for the RTOS.
 * Uses driver layer (rcc, gpio, systick) for hardware access.
 */

#include <stdint.h>
#include "port.h"
#include "rcc.h"
#include "gpio.h"
#include "systick.h"

// =============================================================================
// Base Addresses (STM32F4)
// =============================================================================
#define RCC_BASE        0x40023800
#define PWR_BASE        0x40007000
#define FLASH_BASE      0x40023C00

// =============================================================================
// RCC Registers (STM32F4)
// =============================================================================
#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR     (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44))
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_CSR         (*(volatile uint32_t *)(RCC_BASE + 0x74))

// =============================================================================
// PWR Registers (STM32F4)
// =============================================================================
#define PWR_CR          (*(volatile uint32_t *)(PWR_BASE + 0x00))

// =============================================================================
// Flash Registers (STM32F4)
// =============================================================================
#define FLASH_ACR       (*(volatile uint32_t *)(FLASH_BASE + 0x00))

// =============================================================================
// RCC Bit Definitions
// =============================================================================
/* RCC_CR */
#define RCC_CR_HSION        (1 << 0)
#define RCC_CR_HSIRDY       (1 << 1)
#define RCC_CR_PLLON        (1 << 24)
#define RCC_CR_PLLRDY       (1 << 25)

/* RCC_PLLCFGR */
#define RCC_PLLCFGR_PLLM_MASK   (0x3F << 0)    // PLLM[5:0]
#define RCC_PLLCFGR_PLLM_8      (8 << 0)       // Division factor for PLL input
#define RCC_PLLCFGR_PLLSRC_HSI  (0 << 22)      // HSI as PLL source
#define RCC_PLLCFGR_PLLN_MASK   (0x1FF << 6)   // PLLN[8:0]
#define RCC_PLLCFGR_PLLN_168    (168 << 6)     // VCO multiplier
#define RCC_PLLCFGR_PLLP_MASK   (0x3 << 16)    // PLLP[1:0]
#define RCC_PLLCFGR_PLLP_DIV2   (0 << 16)       // SYSCLK = VCO / 2
#define RCC_PLLCFGR_PLLQ_MASK   (0xF << 24)    // PLLQ[3:0]
#define RCC_PLLCFGR_PLLQ_4      (4 << 24)      // USB OTG, SDIO clock divider

/* RCC_CFGR */
#define RCC_CFGR_SW_MASK         (0x3 << 0)     // SW[1:0]
#define RCC_CFGR_SW_HSI          (0x0 << 0)     // HSI as system clock
#define RCC_CFGR_SW_PLL          (0x2 << 0)     // PLL as system clock
#define RCC_CFGR_SWS_MASK        (0x3 << 2)     // SWS[1:0]
#define RCC_CFGR_HPRE_MASK       (0xF << 4)     // HPRE[3:0]
#define RCC_CFGR_HPRE_DIV1       (0x0 << 4)     // HCLK = SYSCLK / 1
#define RCC_CFGR_PPRE1_MASK      (0x7 << 10)    // PPRE1[2:0]
#define RCC_CFGR_PPRE1_DIV4      (0x5 << 10)   // APB1 = HCLK / 4 (max 42MHz)
#define RCC_CFGR_PPRE2_MASK      (0x7 << 13)   // PPRE2[2:0]
#define RCC_CFGR_PPRE2_DIV2      (0x4 << 13)   // APB2 = HCLK / 2 (max 84MHz)

/* PWR_CR */
#define PWR_CR_VOS_MASK         (0x3 << 14)    // VOS[1:0]
#define PWR_CR_VOS_SCALE1       (0x1 << 14)    // Scale 1 mode (default)

/* FLASH_ACR */
#define FLASH_ACR_LATENCY_MASK  (0xF << 0)     // LATENCY[2:0]
#define FLASH_ACR_LATENCY_5WS   (0x5 << 0)     // 5 wait states for 168MHz
#define FLASH_ACR_PRFTEN        (1 << 8)        // Prefetch enable
#define FLASH_ACR_ICEN          (1 << 9)        // Instruction cache enable
#define FLASH_ACR_DCEN          (1 << 10)       // Data cache enable

/* System clock frequency (for SysTick calculation) */
#define SystemCoreClock         168000000

/* System tick counter */
volatile uint32_t systemTick = 0;

/**
 * @brief Configure System Clock to 168MHz using PLL with HSI
 *
 * Configuration:
 * - HSI: 16MHz internal oscillator
 * - PLL: Enabled, source HSI, PLLM=8, PLLN=168, PLLP=DIV2, PLLQ=4
 * - SYSCLK: 168MHz (16MHz / 8 * 168 / 2)
 * - HCLK: 168MHz (AHB divider = 1)
 * - PCLK1: 42MHz (APB1 divider = 4)
 * - PCLK2: 84MHz (APB2 divider = 2)
 * - Flash latency: 5 wait states
 */
void SystemClock_Config(void) {
    /* Step 1: Enable PWR clock and configure voltage scaling */
    volatile uint32_t *RCC_APB1ENR = (volatile uint32_t *)(RCC_BASE + 0x40);
    *RCC_APB1ENR |= (1 << 28);  // PWREN bit

    PWR_CR = (PWR_CR & ~PWR_CR_VOS_MASK) | PWR_CR_VOS_SCALE1;

    /* Step 2: Configure Flash latency (5 wait states for 168MHz) */
    FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | FLASH_ACR_LATENCY_5WS;
    FLASH_ACR |= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

    /* Step 3: Configure PLL
     * - PLLM = 8: HSI 16MHz / 8 = 2MHz input to VCO
     * - PLLN = 168: VCO = 2MHz * 168 = 336MHz
     * - PLLP = 2: SYSCLK = 336MHz / 2 = 168MHz
     * - PLLQ = 4: USB clock = 336MHz / 4 = 84MHz
     */
    RCC_PLLCFGR = (RCC_PLLCFGR & ~(RCC_PLLCFGR_PLLM_MASK | RCC_PLLCFGR_PLLSRC_HSI |
                                   RCC_PLLCFGR_PLLN_MASK | RCC_PLLCFGR_PLLP_MASK |
                                   RCC_PLLCFGR_PLLQ_MASK))
                | RCC_PLLCFGR_PLLM_8
                | RCC_PLLCFGR_PLLN_168
                | RCC_PLLCFGR_PLLP_DIV2
                | RCC_PLLCFGR_PLLQ_4;

    /* Step 4: Configure bus prescalers
     * - AHB: HCLK = SYSCLK / 1 = 168MHz
     * - APB1: PCLK1 = HCLK / 4 = 42MHz (max 42MHz)
     * - APB2: PCLK2 = HCLK / 2 = 84MHz (max 84MHz)
     */
    RCC_CFGR = (RCC_CFGR & ~(RCC_CFGR_HPRE_MASK | RCC_CFGR_PPRE1_MASK | RCC_CFGR_PPRE2_MASK))
             | RCC_CFGR_HPRE_DIV1
             | RCC_CFGR_PPRE1_DIV4
             | RCC_CFGR_PPRE2_DIV2;

    /* Step 5: Enable PLL */
    RCC_CR |= RCC_CR_PLLON;

    /* Step 6: Wait for PLL to be ready */
    while (!(RCC_CR & RCC_CR_PLLRDY));

    /* Step 7: Select PLL as system clock source */
    RCC_CFGR = (RCC_CFGR & ~RCC_CFGR_SW_MASK) | RCC_CFGR_SW_PLL;

    /* Step 8: Wait for PLL to be used as system clock */
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != (RCC_CFGR_SW_PLL << 2));
}

/**
 * @brief Initialize system hardware
 *
 * Configuration steps:
 * 1. Initialize GPIO pins (PD12, PD13, PD14, PD15) for 4 built-in LEDs
 * 2. Initialize all LEDs to OFF state
 */
void systemInit(void) {
    /* Configure system clock to 168MHz */
    SystemClock_Config();

    /* Configure GPIO pins as output using STM32F4 API */
    /* PD12, PD13, PD14, PD15 (built-in LEDs on STM32F4 Discovery) */
    gpioInitPinSimple(GPIO_PORT_D, 12, GPIO_MODE_OUTPUT);
    gpioInitPinSimple(GPIO_PORT_D, 13, GPIO_MODE_OUTPUT);
    gpioInitPinSimple(GPIO_PORT_D, 14, GPIO_MODE_OUTPUT);
    gpioInitPinSimple(GPIO_PORT_D, 15, GPIO_MODE_OUTPUT);

    /* Initialize LED state: all OFF */
    gpioWritePin(GPIO_PORT_D, 12, 0);
    gpioWritePin(GPIO_PORT_D, 13, 0);
    gpioWritePin(GPIO_PORT_D, 14, 0);
    gpioWritePin(GPIO_PORT_D, 15, 0);
}

/**
 * @brief Initialize SysTick timer for 1ms periodic interrupts
 */
void sysTickInit(void) {
    /* Calculate ticks per millisecond: 168MHz / 1000 = 168000 */
    uint32_t ticksPerMs = SystemCoreClock / 1000;
    systickInit(ticksPerMs);
}

/**
 * @brief SysTick interrupt handler - called every 1ms
 *
 * This is the system tick interrupt that provides:
 * 1. Global millisecond counter (systemTick)
 * 2. LED update callback
 * 3. Task scheduler update
 * 4. Timer tick update
 */
void SysTick_Handler(void) {
    /* Increment global system tick counter */
    systemTick++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = systemTick;
    while ((systemTick - start) < ms);
}

uint32_t getSystemTick(void) {
    return systemTick;
}

/* Context switching functions */
void triggerPendSV(void) {
    /* Trigger PendSV interrupt for context switch */
    volatile uint32_t *SHPR3 = (volatile uint32_t *)0xE000ED20;
    *SHPR3 |= (0xFF << 16);  /* Set PendSV priority */
    volatile uint32_t *ICSR = (volatile uint32_t *)0xE000ED04;
    *ICSR = (1 << 28);       /* Trigger PendSV */
}

uint32_t getCurrentPSP(void) {
    uint32_t result;
    __asm volatile ("mrs %0, psp" : "=r" (result));
    return result;
}

void setCurrentPSP(uint32_t psp) {
    __asm volatile ("msr psp, %0" : : "r" (psp));
}