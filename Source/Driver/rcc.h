/**
 * @file rcc.h
 * @brief STM32F1 RCC (Reset & Clock Control) Driver
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>

/* =============================================================================
 * RCC APB2ENR Clock Enable Bits
 * ============================================================================= */
#define RCC_IOPAEN     (1 << 2)
#define RCC_IOPBEN     (1 << 3)
#define RCC_IOPCEN     (1 << 4)
#define RCC_ADC1EN     (1 << 9)
#define RCC_USART1EN   (1 << 14)

/**
 * @brief Configure System Clock to 72MHz from external 8MHz crystal
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
void SystemClock_Config(void);

/**
 * @brief Enable clock for peripheral
 * @param peripheral: Bit position of peripheral in APB2ENR register
 */
void rccEnableClock(uint32_t peripheral);

/**
 * @brief Disable clock for peripheral
 * @param peripheral: Bit position of peripheral in APB2ENR register
 */
void rccDisableClock(uint32_t peripheral);

#endif /* RCC_H */