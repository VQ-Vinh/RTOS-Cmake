/**
 * @file systick.c
 * @brief SysTick Driver Implementation
 */

#include "systick.h"

/* ========== Địa chỉ cơ sở SysTick ========== */
#define SysTick_BASE    0xE000E010

/* ========== Thanh ghi SysTick ========== */
#define SysTick_CTRL    (*(volatile uint32_t *)(SysTick_BASE + 0x00))
#define SysTick_LOAD    (*(volatile uint32_t *)(SysTick_BASE + 0x04))
#define SysTick_VAL     (*(volatile uint32_t *)(SysTick_BASE + 0x08))

/* ========== Control bits ========== */
#define SYSTICK_CTRL_ENABLE     (1 << 0)   /* Counter enable */
#define SYSTICK_CTRL_TICKINT   (1 << 1)   /* Interrupt enable */
#define SYSTICK_CTRL_CLKSOURCE (1 << 2)   /* Clock source: processor clock */
#define SYSTICK_CTRL_COUNTFLAG (1 << 16)  /* Count flag */

/* ========== Biến ========== */
volatile uint32_t systemTick = 0;  /* Counter tăng mỗi 1ms */

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo SysTick
 * @param ticks: Số ticks reload = SystemCoreClock/1000 = 72000
 *
 * LOAD = ticks - 1 (vì counter đếm xuống từ LOAD về 0)
 * VAL = 0 (xóa counter hiện tại)
 * CTRL = enable + interrupt + processor clock
 */
void systickInit(uint32_t ticks) {
    SysTick_LOAD = ticks - 1;  /* Reload value */
    SysTick_VAL = 0;           /* Clear current value */
    SysTick_CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

void systickStart(void) {
    SysTick_CTRL |= SYSTICK_CTRL_ENABLE;
}

void systickStop(void) {
    SysTick_CTRL &= ~SYSTICK_CTRL_ENABLE;
}

void systickEnableInt(void) {
    SysTick_CTRL |= SYSTICK_CTRL_TICKINT;
}

void systickDisableInt(void) {
    SysTick_CTRL &= ~SYSTICK_CTRL_TICKINT;
}

uint32_t systickGetValue(void) {
    return SysTick_VAL;
}

uint8_t systickGetCountFlag(void) {
    return (SysTick_CTRL >> 16) & 0x1;
}

uint32_t getSystemTick(void) {
    return systemTick;
}

/**
 * @brief Delay milliseconds (dùng systemTick)
 * @param ms: Số ms cần delay
 */
void delay_ms(uint32_t ms) {
    uint32_t start = systemTick;
    while ((systemTick - start) < ms);  /* Chờ đến khi đủ ms */
}

/**
 * @brief Delay microseconds (dùng SysTick counter)
 * @param us: Số us cần delay
 *
 * Đếm số ticks đã trôi qua kể từ start
 */
void systickDelayUs(uint32_t us) {
    volatile uint32_t start = SysTick_VAL;
    uint32_t ticks = (SystemCoreClock / 1000000) * us;
    if (ticks == 0) ticks = 1;

    while (1) {
        volatile uint32_t current = SysTick_VAL;
        uint32_t elapsed;
        if (current <= start) {
            elapsed = start - current;
        } else {
            elapsed = start + (SysTick_LOAD + 1) - current;
        }
        if (elapsed >= ticks) break;
    }
}

/**
 * @brief Delay milliseconds (simple loop)
 * @param ms: Số ms cần delay
 */
void systickDelayMs(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}
