/**
 * @file rcc.c
 * @brief RCC Driver Implementation
 */

#include "rcc.h"

/* ========== Địa chỉ cơ sở ========== */
#define RCC_BASE        0x40021000
#define FLASH_BASE      0x40022000

/* ========== Thanh ghi RCC ========== */
#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

/* ========== Thanh ghi Flash ========== */
#define FLASH_ACR        (*(volatile uint32_t *)(FLASH_BASE + 0x00))

/* ========== RCC CR bits ========== */
#define RCC_CR_HSEON    (1 << 16)   /* HSE enable */
#define RCC_CR_HSERDY   (1 << 17)   /* HSE ready flag */
#define RCC_CR_PLLON    (1 << 24)   /* PLL enable */
#define RCC_CR_PLLRDY   (1 << 25)   /* PLL ready flag */

/* ========== RCC CFGR bits ========== */
#define RCC_CFGR_SW_MASK      (0x3 << 0)     /* System clock switch */
#define RCC_CFGR_SW_PLL       (0x2 << 0)     /* PLL as system clock */
#define RCC_CFGR_SWS_MASK     (0x3 << 2)     /* System clock status */
#define RCC_CFGR_PLLSRC       (1 << 16)      /* PLL source: HSE */
#define RCC_CFGR_PLLMUL_MASK  (0xF << 18)    /* PLL multiplier mask */
#define RCC_CFGR_PLLMUL_9     (0x7 << 18)     /* PLL * 9 */
#define RCC_CFGR_HPRE_DIV1    (0x0 << 4)     /* HCLK = SYSCLK / 1 */
#define RCC_CFGR_PPRE1_DIV2   (0x4 << 8)     /* APB1 = HCLK / 2 */
#define RCC_CFGR_PPRE2_DIV1   (0x0 << 11)     /* APB2 = HCLK / 1 */

/* ========== API Implementation ========== */

/**
 * @brief Cấu hình clock 72MHz từ thạch anh 8MHz
 *
 * Quy trình:
 * 1. Set flash latency (2 wait states)
 * 2. Enable HSE và chờ ready
 * 3. Config PLL (multiplier = 9)
 * 4. Config bus prescalers
 * 5. Enable PLL và chờ ready
 * 6. Switch system clock sang PLL
 */
void SystemClock_Config(void) {
    /* 1. Set flash latency cho 72MHz (2 wait states) */
    FLASH_ACR = (FLASH_ACR & ~(0x7 << 0)) | (0x2 << 0);

    /* 2. Enable HSE */
    RCC_CR |= RCC_CR_HSEON;
    while (!(RCC_CR & RCC_CR_HSERDY));  /* Chờ HSE ready */

    /* 3. Config PLL: HSE * 9 = 72MHz */
    RCC_CFGR = (RCC_CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL_MASK))
             | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL_9;

    /* 4. Config bus prescalers */
    RCC_CFGR = (RCC_CFGR & ~(0xF << 4 | 0x7 << 8 | 0x7 << 11))
             | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1;

    /* 5. Enable PLL */
    RCC_CR |= RCC_CR_PLLON;
    while (!(RCC_CR & RCC_CR_PLLRDY));  /* Chờ PLL ready */

    /* 6. Switch clock sang PLL */
    RCC_CFGR = (RCC_CFGR & ~RCC_CFGR_SW_MASK) | RCC_CFGR_SW_PLL;
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != (RCC_CFGR_SW_PLL << 2));
}

/**
 * @brief Bật clock peripheral
 * @param peripheral: Bit mask trong APB2ENR
 *
 *Ví dụ: rccEnableClock(RCC_IOPAEN) -> bật GPIOA clock
 */
void rccEnableClock(uint32_t peripheral) {
    RCC_APB2ENR |= peripheral;
}

/**
 * @brief Tắt clock peripheral
 */
void rccDisableClock(uint32_t peripheral) {
    RCC_APB2ENR &= ~peripheral;
}
