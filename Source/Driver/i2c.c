/**
 * @file i2c.c
 * @brief I2C Driver Implementation
 */

#include "i2c.h"
#include "../Port/STM32/rcc.h"
#include "../Port/STM32/gpio.h"

/* ========== Địa chỉ cơ sở ========== */
#define I2C1_BASE       0x40005400
#define I2C2_BASE       0x40005800
#define GPIO_BASE       0x40010800
#define RCC_BASE        0x40021000

/* ========== Thanh ghi I2C ========== */
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

/* ========== RCC bits ========== */
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x1C))

#define RCC_I2C1_EN    (1 << 21)
#define RCC_I2C2_EN    (1 << 22)
#define RCC_AFIO_EN    (1 << 0)
#define RCC_GPIO_B_EN  (1 << 3)
#define RCC_GPIO_A_EN  (1 << 2)

/* ========== GPIO Registers ========== */
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

/* ========== Helper functions ========== */

/**
 * @brief Simple delay loop
 */
static void delay(volatile uint32_t count) {
    while (count--) { __asm("nop"); }
}

/**
 * @brief Chờ flag I2C đạt trạng thái mong muốn
 * @param flag: Bit flag cần kiểm tra
 * @param state: 1 = flag set, 0 = flag clear
 * @param timeout: Số lần thử tối đa
 * @return: 0 = thành công, 1 = timeout
 */
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

/**
 * @brief Config GPIO pins cho I2C
 *
 * I2C1: PB6=SCL, PB7=SDA (Alternate open-drain)
 * I2C2: PB10=SCL, PB11=SDA (Alternate open-drain)
 */
static void i2cConfigPins(I2C_Peripheral_t i2c) {
    if (i2c == I2C1) {
        /* I2C1: PB6 (SCL), PB7 (SDA) */
        rccEnableClock(RCC_GPIO_B_EN);
        rccEnableClock(RCC_AFIO_EN);

        /* Config PB6, PB7 as Alt open-drain 50MHz */
        GPIOB->CRL &= ~0xFF000000;
        GPIOB->CRL |= 0xFF000000;
    } else {
        /* I2C2: PB10 (SCL), PB11 (SDA) */
        rccEnableClock(RCC_GPIO_B_EN);
        rccEnableClock(RCC_AFIO_EN);

        GPIOB->CRH &= ~0xFFFF;
        GPIOB->CRH |= 0xFFFF;
    }
}

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo I2C peripheral
 *
 * Quy trình:
 * 1. Enable clocks cho I2C và GPIO
 * 2. Config pins (Alt open-drain)
 * 3. Disable I2C để config
 * 4. Tính CCR và TRISE theo tốc độ
 * 5. Enable I2C
 */
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

    /* Config pins */
    i2cConfigPins(i2c);

    /* Disable I2C during config */
    i2cx->CR1 &= ~0x0001;

    /* PCLK1 = 36MHz for STM32F1 */
    pclk1 = 36000000;

    /* Tính CCR và TRISE theo speed mode */
    if (speed == I2C_SPEED_STANDARD) {
        ccr = pclk1 / (speed * 2);            /* Standard: FCLK/(2*speed) */
        trise = (pclk1 / 1000000) + 1;        /* 1000ns max rise time */
    } else {
        /* Fast mode: duty cycle low/high = 2/1 */
        ccr = pclk1 / (speed * 3);
        trise = (pclk1 / 1000000) + 1;
        ccr |= 0x8000;                        /* Set F/S bit for fast mode */
    }

    i2cx->CCR = ccr;
    i2cx->TRISE = trise;

    /* Enable I2C */
    i2cx->CR1 |= 0x0001;

    delay(10);  /* Chờ bus ready */
}

/**
 * @brief Ghi 1 byte vào thanh ghi
 *
 * Quy trình I2C write:
 * 1. START
 * 2. Send device address (write)
 * 3. Clear ADDR flag
 * 4. Send register address
 * 5. Send data
 * 6. STOP
 */
uint8_t i2cWriteReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;  /* Convert 7-bit to 8-bit address (R/W=0) */

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

/**
 * @brief Đọc 1 byte từ thanh ghi
 *
 * Quy trình I2C read:
 * 1. START
 * 2. Send device address (write) - để gửi regAddr
 * 3. Send register address
 * 4. Repeated START
 * 5. Send device address (read)
 * 6. Disable ACK, generate STOP
 * 7. Read data
 */
uint8_t i2cReadReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;

    /* START */
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

    /* Repeated START */
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

/**
 * @brief Ghi buffer nhiều bytes
 */
uint8_t i2cWriteBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;

    if (len == 0) return 1;

    /* START */
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

    /* Wait for BTF (byte transfer finished) */
    if (i2cWaitFlag(i2cx, 0x0004, 1, 1000)) return 1;

    /* STOP */
    i2cx->CR1 |= 0x0200;
    delay(10);

    return 0;
}

/**
 * @brief Đọc buffer nhiều bytes
 */
uint8_t i2cReadBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len) {
    I2C_TypeDef *i2cx = (i2c == I2C1) ? I2C1 : I2C2;
    devAddr <<= 1;

    if (len == 0) return 1;

    /* START */
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

    /* Repeated START */
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
            /* Disable ACK for last byte, generate STOP */
            i2cx->CR1 &= ~0x0400;
            i2cx->CR1 |= 0x0200;
        }
    }

    delay(10);
    return 0;
}
