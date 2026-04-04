/**
 * @file i2c.c
 * @brief STM32F1 I2C Driver Implementation
 */

#include "i2c.h"
#include "../Port/STM32/rcc.h"
#include "../Port/STM32/gpio.h"

// =============================================================================
// Base Addresses
// =============================================================================
#define I2C1_BASE       0x40005400
#define I2C2_BASE       0x40005800
#define GPIO_BASE       0x40010800
#define RCC_BASE        0x40021000

// =============================================================================
// I2C Registers
// =============================================================================
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
} I2C_TypeDef;

#define I2C1        ((I2C_TypeDef *)I2C1_BASE)
#define I2C2        ((I2C_TypeDef *)I2C2_BASE)

// =============================================================================
// RCC Register Definitions
// =============================================================================
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x1C))

// RCC clock enable bits
#define RCC_I2C1_EN    (1 << 21)
#define RCC_I2C2_EN    (1 << 22)
#define RCC_AFIO_EN    (1 << 0)
#define RCC_GPIO_B_EN  (1 << 3)
#define RCC_GPIO_A_EN  (1 << 2)

// =============================================================================
// GPIO Registers
// =============================================================================
typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;

#define GPIOA         ((GPIO_TypeDef *)GPIO_BASE)
#define GPIOB         ((GPIO_TypeDef *)(GPIO_BASE + 0x0400))

// =============================================================================
// Delay Function (simple loop)
// =============================================================================
static void delay(volatile uint32_t count) {
    while (count--) { __asm("nop"); }
}

// =============================================================================
// I2C Wait Flag
// =============================================================================
static uint8_t i2cWaitFlag(I2C_TypeDef *i2c, uint32_t flag, uint32_t state, uint32_t timeout) {
    volatile uint32_t tick = 0;
    uint32_t sr1;

    while (tick < timeout) {
        sr1 = i2c->SR1;
        if (state) {
            if (sr1 & flag) return 0;
        } else {
            if (!(sr1 & flag)) return 0;
        }
        tick++;
    }
    return 1;
}

// =============================================================================
// I2C Hardware Configuration
// =============================================================================
static void i2cConfigPins(I2C_Peripheral_t i2c) {
    if (i2c == I2C1) {
        /* I2C1: PB6 (SCL), PB7 (SDA) */
        rccEnableClock(RCC_GPIO_B_EN);
        rccEnableClock(RCC_AFIO_EN);

        /* SCL: PB6 - Alternate function open-drain 50MHz */
        GPIOB->CRL &= ~0xFF000000;  /* Clear bits for PB6, PB7 */
        GPIOB->CRL |= 0xFF000000;   /* Set to alternate function open-drain */
    } else {
        /* I2C2: PB10 (SCL), PB11 (SDA) */
        rccEnableClock(RCC_GPIO_B_EN);
        rccEnableClock(RCC_AFIO_EN);

        /* SCL: PB10, SDA: PB11 */
        GPIOB->CRH &= ~0xFFFF;
        GPIOB->CRH |= 0xFFFF;
    }
}

// =============================================================================
// API Implementation
// =============================================================================
void i2cInit(I2C_Peripheral_t i2c, I2C_Speed_t speed) {
    I2C_TypeDef *i2cx;
    uint32_t ccr;
    uint32_t trise;
    uint32_t pclk1;

    if (i2c == I2C1) {
        i2cx = I2C1;
        rccEnableClock(RCC_I2C1_EN);
    } else {
        i2cx = I2C2;
        rccEnableClock(RCC_I2C2_EN);
    }

    /* Configure pins */
    i2cConfigPins(i2c);

    /* Disable I2C before configuration */
    i2cx->CR1 &= ~0x0001;

    /* PCLK1 is 36MHz for STM32F1 */
    pclk1 = 36000000;

    /* Configure clock control */
    if (speed == I2C_SPEED_STANDARD) {
        ccr = pclk1 / (speed * 2);
        trise = (pclk1 / 1000000) + 1;  /* 1000ns max rise time */
    } else {
        /* Fast mode: duty cycle 0 (low/high = 2/1) */
        ccr = pclk1 / (speed * 3);
        trise = (pclk1 / 1000000) + 1;
        ccr |= 0x8000;  /* Set F/S bit for fast mode */
    }

    i2cx->CCR = ccr;
    i2cx->TRISE = trise;

    /* Enable I2C */
    i2cx->CR1 |= 0x0001;

    /* Wait for SB flag to clear (bus ready) */
    delay(10);
}

uint8_t i2cWriteReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;  /* Convert to 8-bit address (R/W bit = 0) */

    /* Generate START */
    i2cx->CR1 |= 0x0100;
    if (i2cWaitFlag(i2cx, 0x0001, 1, 1000)) return 1;  /* Wait SB */

    /* Send device address */
    i2cx->DR = devAddr & 0xFE;
    if (i2cWaitFlag(i2cx, 0x0002, 1, 1000)) return 1;  /* Wait ADDR */

    /* Clear ADDR flag */
    (void)i2cx->SR1;
    (void)i2cx->SR2;

    /* Send register address */
    i2cx->DR = regAddr;
    if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;  /* Wait TxE */

    /* Send data */
    i2cx->DR = data;
    if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;

    /* Generate STOP */
    i2cx->CR1 |= 0x0200;
    delay(10);

    return 0;
}

uint8_t i2cReadReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;

    /* Generate START */
    i2cx->CR1 |= 0x0100;
    if (i2cWaitFlag(i2cx, 0x0001, 1, 1000)) return 1;

    /* Send device address (write mode) */
    i2cx->DR = devAddr & 0xFE;
    if (i2cWaitFlag(i2cx, 0x0002, 1, 1000)) return 1;

    (void)i2cx->SR1;
    (void)i2cx->SR2;

    /* Send register address */
    i2cx->DR = regAddr;
    if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;

    /* Generate repeated START */
    i2cx->CR1 |= 0x0100;
    if (i2cWaitFlag(i2cx, 0x0001, 1, 1000)) return 1;

    /* Send device address (read mode) */
    i2cx->DR = devAddr | 0x01;
    if (i2cWaitFlag(i2cx, 0x0002, 1, 1000)) return 1;

    (void)i2cx->SR1;
    (void)i2cx->SR2;

    /* Disable ACK and generate STOP */
    i2cx->CR1 &= ~0x0400;
    i2cx->CR1 |= 0x0200;

    /* Read data */
    *data = i2cx->DR;
    delay(10);

    return 0;
}

uint8_t i2cWriteBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;

    if (len == 0) return 1;

    /* Generate START */
    i2cx->CR1 |= 0x0100;
    if (i2cWaitFlag(i2cx, 0x0001, 1, 1000)) return 1;

    /* Send device address */
    i2cx->DR = devAddr & 0xFE;
    if (i2cWaitFlag(i2cx, 0x0002, 1, 1000)) return 1;

    (void)i2cx->SR1;
    (void)i2cx->SR2;

    /* Send register address */
    i2cx->DR = regAddr;
    if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;

    /* Send data buffer */
    while (len--) {
        if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;
        i2cx->DR = *buffer++;
    }

    /* Wait for BTF */
    if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;

    /* Generate STOP */
    i2cx->CR1 |= 0x0200;
    delay(10);

    return 0;
}

uint8_t i2cReadBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;

    if (len == 0) return 1;

    /* Generate START */
    i2cx->CR1 |= 0x0100;
    if (i2cWaitFlag(i2cx, 0x0001, 1, 1000)) return 1;

    /* Send device address (write) */
    i2cx->DR = devAddr & 0xFE;
    if (i2cWaitFlag(i2cx, 0x0002, 1, 1000)) return 1;

    (void)i2cx->SR1;
    (void)i2cx->SR2;

    /* Send register address */
    i2cx->DR = regAddr;
    if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;

    /* Generate repeated START */
    i2cx->CR1 |= 0x0100;
    if (i2cWaitFlag(i2cx, 0x0001, 1, 1000)) return 1;

    /* Send device address (read) */
    i2cx->DR = devAddr | 0x01;
    if (i2cWaitFlag(i2cx, 0x0002, 1, 1000)) return 1;

    (void)i2cx->SR1;
    (void)i2cx->SR2;

    /* Enable ACK */
    i2cx->CR1 |= 0x0400;

    /* Read data */
    while (len--) {
        if (i2cWaitFlag(i2cx, 0x0040, 1, 1000)) return 1;  /* Wait RxNE */
        *buffer++ = i2cx->DR;
        if (len == 1) {
            i2cx->CR1 &= ~0x0400;  /* Disable ACK for last byte */
            i2cx->CR1 |= 0x0200;   /* Generate STOP */
        }
    }

    delay(10);
    return 0;
}