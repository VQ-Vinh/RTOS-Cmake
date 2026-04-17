/**
 * @file uart.c
 * @brief STM32F1 UART Driver Implementation
 */

#include "uart.h"
#include "../Port/STM32/rcc.h"
#include <stdarg.h>
#include <string.h>

// =============================================================================
// Base Addresses
// =============================================================================
#define UART1_BASE      0x40013800
#define UART2_BASE      0x40004400
#define UART3_BASE      0x40004800
#define GPIO_BASE       0x40010800
#define RCC_BASE        0x40021000

// =============================================================================
// UART Registers
// =============================================================================
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} UART_TypeDef;

#define UART1       ((UART_TypeDef *)UART1_BASE)
#define UART2       ((UART_TypeDef *)UART2_BASE)
#define UART3       ((UART_TypeDef *)UART3_BASE)

// =============================================================================
// RCC Register Definitions
// =============================================================================
#define RCC_APB2ENR    (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x1C))

/* RCC APB2ENR bits */
#define RCC_UART1_EN   (1 << 14)
#define RCC_AFIO_EN    (1 << 0)
#define RCC_GPIO_A_EN  (1 << 2)
#define RCC_GPIO_B_EN  (1 << 3)
#define RCC_GPIO_C_EN  (1 << 4)

/* RCC APB1ENR bits */
#define RCC_UART2_EN   (1 << 17)
#define RCC_UART3_EN   (1 << 18)

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
#define GPIOC         ((GPIO_TypeDef *)(GPIO_BASE + 0x0800))

// =============================================================================
// System Core Clock (must match port.c)
// =============================================================================
#ifndef SystemCoreClock
#define SystemCoreClock 72000000
#endif

// =============================================================================
// Static Functions
// =============================================================================
static void uartConfigPins(UART_Peripheral_t uart) {
    if (uart == UART_PERIPHERAL_1) {
        /* UART1: PA9 (TX), PA10 (RX) */
        rccEnableClock(RCC_GPIO_A_EN);

        /* TX: PA9 - Alternate function push-pull 50MHz */
        GPIOA->CRH &= ~0x0000F00F;
        GPIOA->CRH |= 0x0000B00B;   /* TX: Alt PP 50MHz, RX: Input floating */
    } else if (uart == UART_PERIPHERAL_2) {
        /* UART2: PA2 (TX), PA3 (RX) */
        rccEnableClock(RCC_GPIO_A_EN);

        /* TX: PA2, RX: PA3 */
        GPIOA->CRL &= ~0xFF00;
        GPIOA->CRL |= 0xBB00;
    } else {
        /* UART3: PC10 (TX), PC11 (RX) */
        rccEnableClock(RCC_GPIO_C_EN);

        /* TX: PC10, RX: PC11 */
        GPIOC->CRH &= ~0xFFFF;
        GPIOC->CRH |= 0xFFFF;
    }
}

static uint32_t uartCalcBaudrate(uint32_t baudrate) {
    /* BRR = APB2Clock / baudrate for UART1 on APB2 */
    return SystemCoreClock / baudrate;
}

static UART_TypeDef* uartGetBase(UART_Peripheral_t uart) {
    switch (uart) {
        case UART_PERIPHERAL_1: return UART1;
        case UART_PERIPHERAL_2: return UART2;
        case UART_PERIPHERAL_3: return UART3;
        default: return UART1;
    }
}

static void uartEnableClock(UART_Peripheral_t uart) {
    if (uart == UART_PERIPHERAL_1) {
        rccEnableClock(RCC_UART1_EN);
    } else if (uart == UART_PERIPHERAL_2) {
        rccEnableClock(RCC_UART2_EN);
    } else {
        rccEnableClock(RCC_UART3_EN);
    }
}

// =============================================================================
// API Implementation
// =============================================================================
void uartInit(UART_Peripheral_t uart, UART_Baudrate_t baudrate) {
    UART_TypeDef *uartx = uartGetBase(uart);

    /* Enable clock */
    uartEnableClock(uart);
    rccEnableClock(RCC_AFIO_EN);

    /* Configure pins */
    uartConfigPins(uart);

    /* Disable UART before configuration */
    uartx->CR1 = 0x0000;

    /* Set baudrate */
    uartx->BRR = uartCalcBaudrate(baudrate);

    /* Enable TX and RX, 8 data bits */
    uartx->CR1 = 0x000C;  /* TE=1, RE=1 */

    /* Enable UART */
    uartx->CR1 |= 0x2000;  /* UE=1 */
}

void uartPutChar(UART_Peripheral_t uart, uint8_t ch) {
    UART_TypeDef *uartx = uartGetBase(uart);

    /* Wait for TX buffer empty (TXE) */
    while (!(uartx->SR & 0x80)) { }
    uartx->DR = ch;

    /* Wait for transmission complete (TC) */
    while (!(uartx->SR & 0x40)) { }
}

void uartPutString(UART_Peripheral_t uart, const char *str) {
    while (*str) {
        uartPutChar(uart, *str++);
    }
}

uint8_t uartGetChar(UART_Peripheral_t uart) {
    UART_TypeDef *uartx = uartGetBase(uart);

    /* Wait for data received (RXNE) */
    while (!(uartx->SR & 0x0020)) { }
    return (uint8_t)(uartx->DR & 0xFF);
}

uint8_t uartAvailable(UART_Peripheral_t uart) {
    UART_TypeDef *uartx = uartGetBase(uart);
    return (uartx->SR & 0x0020) ? 1 : 0;
}

/**
 * @brief Simple integer to string conversion
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
 * @brief Simple printf implementation
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