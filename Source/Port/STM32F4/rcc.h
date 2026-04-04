/**
 * @file rcc.h
 * @brief STM32F4 RCC (Reset & Clock Control) Driver
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>

/**
 * @brief Enable clock for peripheral
 * @param peripheral: Bit position of peripheral in AHB1ENR register
 */
void rccEnableClock(uint32_t peripheral);

/**
 * @brief Disable clock for peripheral
 * @param peripheral: Bit position of peripheral in AHB1ENR register
 */
void rccDisableClock(uint32_t peripheral);

#endif /* RCC_H */