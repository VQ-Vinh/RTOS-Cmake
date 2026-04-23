/**
 * @file rcc.h
 * @brief RCC Driver - Reset & Clock Control
 *
 * Chức năng: Cấu hình clock hệ thống, bật/tắt clock ngoại vi
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>

/* ========== Clock Enable Bits (APB2ENR) ========== */
/* Dùng để bật clock cho các peripheral */
#define RCC_IOPAEN     (1 << 2)   /* GPIO Port A */
#define RCC_IOPBEN     (1 << 3)   /* GPIO Port B */
#define RCC_IOPCEN     (1 << 4)   /* GPIO Port C */
#define RCC_ADC1EN     (1 << 9)   /* ADC1 */
#define RCC_USART1EN   (1 << 14)   /* USART1 */

/* ========== Hàm API ========== */

/**
 * @brief Cấu hình clock hệ thống 72MHz
 *
 * Nguồn clock: HSE 8MHz thạch anh ngoài
 * PLL: 8MHz * 9 = 72MHz
 * Bus: AHB 72MHz, APB1 36MHz, APB2 72MHz
 */
void SystemClock_Config(void);

/**
 * @brief Bật clock peripheral
 * @param peripheral: Bit mask (VD: RCC_IOPAEN)
 *
 * Ví dụ: rccEnableClock(RCC_IOPAEN | RCC_ADC1EN);
 */
void rccEnableClock(uint32_t peripheral);

/**
 * @brief Tắt clock peripheral
 * @param peripheral: Bit mask
 */
void rccDisableClock(uint32_t peripheral);

#endif /* RCC_H */
