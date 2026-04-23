/**
 * @file systick.h
 * @brief SysTick Driver - System Timer
 *
 * Chức năng: Timer 24-bit cho ngắt định kỳ 1ms
 * Cung cấp: biến đếm systemTick, delay functions
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* Tần số core - STM32F1 chạy 72MHz */
#ifndef SystemCoreClock
#define SystemCoreClock   72000000
#endif

/* ========== Biến toàn cục ========== */

/**
 * @brief Bộ đếm system tick (tăng mỗi 1ms)
 * Đọc bằng: getSystemTick()
 */
extern volatile uint32_t systemTick;

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo SysTick cho ngắt 1ms
 * @param ticks: Số ticks = SystemCoreClock / 1000 = 72000
 */
void systickInit(uint32_t ticks);

/**
 * @brief Bắt đầu đếm SysTick
 */
void systickStart(void);

/**
 * @brief Dừng SysTick
 */
void systickStop(void);

/**
 * @brief Bật ngắt SysTick
 */
void systickEnableInt(void);

/**
 * @brief Tắt ngắt SysTick
 */
void systickDisableInt(void);

/**
 * @brief Đọc giá trị hiện tại của SysTick counter
 * @return: Giá trị thanh ghi VAL
 */
uint32_t systickGetValue(void);

/**
 * @brief Kiểm tra flag tràn counter
 * @return: 1 = đã tràn, 0 = chưa
 */
uint8_t systickGetCountFlag(void);

/**
 * @brief Lấy thời gian uptime (ms)
 * @return: systemTick
 */
uint32_t getSystemTick(void);

/**
 * @brief Delay blocking (ms)
 * @param ms: Số milliseconds
 */
void delay_ms(uint32_t ms);

/**
 * @brief Delay blocking (us)
 * @param us: Số microseconds
 */
void systickDelayUs(uint32_t us);

/**
 * @brief Delay blocking (ms) - simple loop
 * @param ms: Số milliseconds
 */
void systickDelayMs(uint32_t ms);

#endif /* SYSTICK_H */
