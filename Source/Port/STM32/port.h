/**
 * @file port.h
 * @brief Port Layer - Abstraction giữa Kernel và Hardware
 *
 * Chức năng: Định nghĩa interface giữa RTOS Kernel và STM32F1 hardware
 * Kernel gọi các hàm này, Port implementation cung cấp code hardware-specific
 */

#ifndef PORT_H
#define PORT_H

#include <stdint.h>

/* ========== System Initialization ========== */

/**
 * @brief Khởi tạo hardware (clocks, GPIO)
 *
 * Gọi SystemClock_Config() và enable GPIO clocks
 */
void systemInit(void);

/* ========== SysTick Timer ========== */

/**
 * @brief Khởi tạo SysTick cho ngắt 1ms
 */
void sysTickInit(void);

/**
 * @brief Lấy system tick counter
 * @return: Số milliseconds đã trôi qua
 */
uint32_t getSystemTick(void);

/**
 * @brief Delay blocking (ms)
 * @param ms: Số milliseconds cần delay
 */
void delay_ms(uint32_t ms);

/* ========== LED Control (optional) ========== */

/**
 * @brief LED callback - được gọi mỗi tick từ App layer
 *
 * Khai báo extern trong file App
 */
extern void ledControlCallback(void);

#endif /* PORT_H */
