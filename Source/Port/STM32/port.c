/**
 * @file port.c
 * @brief Port Layer Implementation cho STM32F1
 *
 * Layer mỏng - ủy thác cho Driver functions
 * Chi tiết hardware (registers, clock config) nằm ở Driver/
 */

#include <stdint.h>
#include "rcc.h"
#include "systick.h"

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo system hardware
 *
 * 1. Cấu hình clock 72MHz (SystemClock_Config trong rcc.c)
 * 2. Enable clocks cho GPIO và peripherals
 */
void systemInit(void) {
    SystemClock_Config();
    rccEnableClock(RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_USART1EN);
}

/**
 * @brief Khởi tạo SysTick cho 1ms tick
 *
 * Gọi systickInit() với SystemCoreClock/1000 = 72000 ticks
 */
void sysTickInit(void) {
    systickInit(SystemCoreClock / 1000);
}

/**
 * @brief SysTick Interrupt Handler
 *
 * Tăng systemTick mỗi 1ms
 * (Hàm này được gọi bởi hardware khi SysTick tràn)
 */
void SysTick_Handler(void) {
    systemTick++;
}
