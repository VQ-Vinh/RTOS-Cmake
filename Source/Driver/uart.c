/**
 * @file uart.c
 * @brief UART Driver Implementation
 */

#include "uart.h"
#include "rcc.h"
#include <stdarg.h>
#include <string.h>

/* ========== Địa chỉ cơ sở ========== */
#define UART1_BASE      0x40013800
#define UART2_BASE      0x40004400
#define UART3_BASE      0x40004800
#define GPIO_BASE       0x40010800
#define RCC_BASE        0x40021000

/* ========== Thanh ghi UART ========== */
typedef struct {
    volatile uint32_t SR;     /* Status register */
    volatile uint32_t DR;     /* Data register */
    volatile uint32_t BRR;    /* Baudrate register */
    volatile uint32_t CR1;    /* Control 1 */
    volatile uint32_t CR2;    /* Control 2 */
    volatile uint32_t CR3;    /* Control 3 */
    volatile uint32_t GTPR;   /* Guard time & prescaler */
} UART_TypeDef;

#define UART1       ((UART_TypeDef *)UART1_BASE)
#define UART2       ((UART_TypeDef *)UART2_BASE)
#define UART3       ((UART_TypeDef *)UART3_BASE)

/* ========== RCC bits ========== */
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x1C))

/* APB2 enable bits */
#define RCC_UART1_EN   (1 << 14)
#define RCC_AFIO_EN    (1 << 0)
#define RCC_GPIO_A_EN  (1 << 2)
#define RCC_GPIO_B_EN  (1 << 3)
#define RCC_GPIO_C_EN  (1 << 4)

/* APB1 enable bits */
#define RCC_UART2_EN   (1 << 17)
#define RCC_UART3_EN   (1 << 18)

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
#define GPIOC         ((GPIO_TypeDef *)(GPIO_BASE + 0x0800))

/* ========== Tần số core ========== */
#ifndef SystemCoreClock
#define SystemCoreClock 72000000
#endif

/* ========== Static functions ========== */

/**
 * @brief Config GPIO pins cho UART
 *
 * UART1: PA9=TX (Alt PP), PA10=RX (Input floating)
 * UART2: PA2=TX, PA3=RX
 * UART3: PC10=TX, PC11=RX
 */
static void uartConfigPins(UART_Peripheral_t uart) {
    if (uart == UART_PERIPHERAL_1) {
        /* UART1: PA9 (TX), PA10 (RX) */
        rccEnableClock(RCC_GPIO_A_EN);
        GPIOA->CRH &= ~0x00000FF0;
        GPIOA->CRH |= 0x000004B0;   /* PA9=Alt PP, PA10=Input */
    } else if (uart == UART_PERIPHERAL_2) {
        /* UART2: PA2 (TX), PA3 (RX) */
        rccEnableClock(RCC_GPIO_A_EN);
        GPIOA->CRL &= ~0xFF00;
        GPIOA->CRL |= 0x4B00;
    } else {
        /* UART3: PC10 (TX), PC11 (RX) */
        rccEnableClock(RCC_GPIO_C_EN);
        GPIOC->CRH &= ~0x0000FF00;
        GPIOC->CRH |= 0x00004B00;
    }
}

/**
 * @brief Tính giá trị BRR cho baudrate
 *
 * BRR format: (Mantissa << 4) | Fraction
 * UART1 on APB2 (72MHz), UART2/3 on APB1 (36MHz)
 */
static uint32_t uartCalcBaudrate(UART_Peripheral_t uart, uint32_t baudrate) {
    uint32_t clock = (uart == UART_PERIPHERAL_1) ? SystemCoreClock : (SystemCoreClock / 2);
    uint32_t divider = clock / baudrate;
    uint32_t mantissa = divider >> 4;
    uint32_t fraction = divider & 0x0F;
    return (mantissa << 4) | fraction;
}

/**
 * @brief Lấy base address của UART
 */
static UART_TypeDef* uartGetBase(UART_Peripheral_t uart) {
    switch (uart) {
        case UART_PERIPHERAL_1: return UART1;
        case UART_PERIPHERAL_2: return UART2;
        case UART_PERIPHERAL_3: return UART3;
        default: return UART1;
    }
}

/**
 * @brief Enable clock cho UART
 */
static void uartEnableClock(UART_Peripheral_t uart) {
    if (uart == UART_PERIPHERAL_1) {
        rccEnableClock(RCC_UART1_EN);
    } else if (uart == UART_PERIPHERAL_2) {
        rccEnableClock(RCC_UART2_EN);
    } else {
        rccEnableClock(RCC_UART3_EN);
    }
}

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo UART
 *
 * 8 data bits, 1 stop bit, no parity
 * TX và RX enabled
 */
void uartInit(UART_Peripheral_t uart, UART_Baudrate_t baudrate) {
    UART_TypeDef *uartx = uartGetBase(uart);

    /* Enable clocks */
    uartEnableClock(uart);
    rccEnableClock(RCC_AFIO_EN);

    /* Config pins */
    uartConfigPins(uart);

    /* Disable UART during config */
    uartx->CR1 = 0x0000;

    /* Set baudrate */
    uartx->BRR = uartCalcBaudrate(uart, (uint32_t)baudrate);

    /* Enable TX và RX */
    uartx->CR1 = 0x000C;

    /* Enable UART */
    uartx->CR1 |= 0x2000;
}

/**
 * @brief Gửi 1 ký tự (blocking)
 *
 * Chờ TX buffer empty (TXE flag), gửi ký tự
 * Chờ transmission complete (TC flag)
 */
void uartPutChar(UART_Peripheral_t uart, uint8_t ch) {
    UART_TypeDef *uartx = uartGetBase(uart);

    while (!(uartx->SR & 0x80));  /* Chờ TXE */
    uartx->DR = ch;

    while (!(uartx->SR & 0x40));  /* Chờ TC */
}

/**
 * @brief Gửi chuỗi string
 */
void uartPutString(UART_Peripheral_t uart, const char *str) {
    while (*str) {
        uartPutChar(uart, *str++);
    }
}

/**
 * @brief Nhận 1 ký tự (blocking)
 */
uint8_t uartGetChar(UART_Peripheral_t uart) {
    UART_TypeDef *uartx = uartGetBase(uart);

    while (!(uartx->SR & 0x0020));  /* Chờ RXNE */
    return (uint8_t)(uartx->DR & 0xFF);
}

/**
 * @brief Kiểm tra có dữ liệu đến
 */
uint8_t uartAvailable(UART_Peripheral_t uart) {
    UART_TypeDef *uartx = uartGetBase(uart);
    return (uartx->SR & 0x0020) ? 1 : 0;
}

/**
 * @brief Convert integer sang string
 */
static void itoa(uint32_t value, char *buffer, uint8_t base) {
    char temp[32];
    uint8_t i = 0, j;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0) {
        uint8_t digit = value % base;
        temp[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= base;
    }

    for (j = 0; j < i; j++) {
        buffer[j] = temp[i - 1 - j];
    }
    buffer[i] = '\0';
}

/**
 * @brief Simple printf for UART
 *
 * Hỗ trợ: %d (int), %u (unsigned), %x (hex), %c (char), %s (string), %%
 */
void uartPrintf(UART_Peripheral_t uart, const char *format, ...) {
    char buffer[32];
    va_list args;
    const char *p;

    va_start(args, format);
    p = format;

    while (*p) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': {
                    int val = va_arg(args, int);
                    itoa((uint32_t)(val < 0 ? -val : val), buffer, 10);
                    if (val < 0) uartPutChar(uart, '-');
                    uartPutString(uart, buffer);
                    break;
                }
                case 'u': {
                    uint32_t val = va_arg(args, uint32_t);
                    itoa(val, buffer, 10);
                    uartPutString(uart, buffer);
                    break;
                }
                case 'x': {
                    uint32_t val = va_arg(args, uint32_t);
                    itoa(val, buffer, 16);
                    uartPutString(uart, buffer);
                    break;
                }
                case 'c': {
                    char val = (char)va_arg(args, int);
                    uartPutChar(uart, val);
                    break;
                }
                case 's': {
                    char *val = va_arg(args, char *);
                    uartPutString(uart, val);
                    break;
                }
                case '%': {
                    uartPutChar(uart, '%');
                    break;
                }
                default:
                    break;
            }
        } else {
            uartPutChar(uart, *p);
        }
        p++;
    }

    va_end(args);
}
