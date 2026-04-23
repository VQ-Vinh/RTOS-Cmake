/**
 * @file port.c
 * @brief STM32F1 Hardware Port
 *
 * Thin abstraction layer - delegates to Driver functions.
 * Hardware-specific details (registers, clock config) live in Driver/.
 */

#include <stdint.h>
#include "rcc.h"
#include "systick.h"

/**
 * @brief Initialize system hardware (clocks, GPIO clocks)
 */
void systemInit(void) {
    SystemClock_Config();
    rccEnableClock(RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_USART1EN);
}

/**
 * @brief Initialize SysTick for 1ms tick
 */
void sysTickInit(void) {
    systickInit(SystemCoreClock / 1000);
}

/**
 * @brief SysTick interrupt handler - called every 1ms
 */
void SysTick_Handler(void) {
    systemTick++;
}