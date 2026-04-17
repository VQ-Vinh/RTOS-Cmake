/**
 * @file uart_f4.c
 * @brief STM32F4 UART Driver Implementation
 */

#include "uart_f4.h"
#include "../../Port/STM32F4/rcc.h"
#include <stdarg.h>

// =============================================================================
// Base Addresses (STM32F4)
// =============================================================================
#define UART1_BASE      0x40011000
#define UART2_BASE      0x40004400
#define UART3_BASE      0x40004800

#define GPIOA_BASE      0x40020000
#define GPIOB_BASE      0x40020400
#define GPIOC_BASE      0x40020800

// =============================================================================
// UART Registers (STM32F4)
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
// GPIO Registers (STM32F4)
// =============================================================================
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_Reg_t;

#define GPIOA   ((GPIO_Reg_t *)GPIOA_BASE)
#define GPIOB   ((GPIO_Reg_t *)GPIOB_BASE)
#define GPIOC   ((GPIO_Reg_t *)GPIOC_BASE)

// =============================================================================
// RCC Register Definitions (STM32F4)
// =============================================================================
#define RCC_BASE        0x40023800
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))

/* RCC APB2ENR bits */
#define RCC_UART1_EN    (1 << 4)
#define RCC_AFIO_EN     (1 << 0)  // Not used for F4, but kept for compatibility

/* RCC APB1ENR bits */
#define RCC_UART2_EN    (1 << 17)
#define RCC_UART3_EN    (1 << 18)

/* RCC AHB1ENR bits */
#define RCC_GPIO_A_EN   (1 << 0)
#define RCC_GPIO_B_EN   (1 << 1)
#define RCC_GPIO_C_EN   (1 << 2)

// =============================================================================
// System Core Clock (F4 runs at 168MHz)
#define SystemCoreClock 168000000

// =============================================================================
// Static Functions
// =============================================================================
static void uartConfigPins(UART_Peripheral_t uart) {
    if (uart == UART_PERIPHERAL_1) {
        /* UART1: PA9 (TX), PA10 (RX) */
        RCC_AHB1ENR |= RCC_GPIO_A_EN;
        /* TX: PA9 - Alternate function push-pull @ 50MHz, RX: PA10 - Floating input */
        GPIOA->MODER &= ~((0x3 << (9 * 2)) | (0x3 << (10 * 2)));
        GPIOA->MODER |= (0x2 << (9 * 2)) | (0x2 << (10 * 2));  // Alt function
        GPIOA->OSPEEDR |= (0x3 << (9 * 2)) | (0x3 << (10 * 2));  // High speed
        GPIOA->AFR[1] &= ~(0xF << ((9 - 8) * 4));
        GPIOA->AFR[1] |= (0x7 << ((9 - 8) * 4));  // AF7 for USART1
        GPIOA->AFR[1] &= ~(0xF << ((10 - 8) * 4));
        GPIOA->AFR[1] |= (0x7 << ((10 - 8) * 4)); // AF7 for USART1
    } else if (uart == UART_PERIPHERAL_2) {
        /* UART2: PA2 (TX), PA3 (RX) */
        RCC_AHB1ENR |= RCC_GPIO_A_EN;
        GPIOA->MODER &= ~((0x3 << (2 * 2)) | (0x3 << (3 * 2)));
        GPIOA->MODER |= (0x2 << (2 * 2)) | (0x2 << (3 * 2));
        GPIOA->OSPEEDR |= (0x3 << (2 * 2)) | (0x3 << (3 * 2));
        GPIOA->AFR[0] &= ~(0xF << (2 * 4));
        GPIOA->AFR[0] |= (0x7 << (2 * 4));   // AF7 for USART2
        GPIOA->AFR[0] &= ~(0xF << (3 * 4));
        GPIOA->AFR[0] |= (0x7 << (3 * 4));  // AF7 for USART2
    } else {
        /* UART3: PC10 (TX), PC11 (RX) */
        RCC_AHB1ENR |= RCC_GPIO_C_EN;
        GPIOC->MODER &= ~((0x3 << (10 * 2)) | (0x3 << (11 * 2)));
        GPIOC->MODER |= (0x2 << (10 * 2)) | (0x2 << (11 * 2));
        GPIOC->OSPEEDR |= (0x3 << (10 * 2)) | (0x3 << (11 * 2));
        GPIOC->AFR[1] &= ~(0xF << ((10 - 8) * 4));
        GPIOC->AFR[1] |= (0x7 << ((10 - 8) * 4));  // AF7 for USART3
        GPIOC->AFR[1] &= ~(0xF << ((11 - 8) * 4));
        GPIOC->AFR[1] |= (0x7 << ((11 - 8) * 4)); // AF7 for USART3
    }
}

static uint32_t uartCalcBaudrate(UART_Peripheral_t uart, uint32_t baudrate) {
    /* BRR register format for F4: (DIV_Mantissa << 4) | FRAC
     * F4 has different clock architecture - UART1/6 on APB2, UART2/3/4/5 on APB1
     * APB2 clock = HCLK = 168MHz (for F4)
     * APB1 clock = HCLK/2 = 84MHz (max 42MHz for UART)
     */
    uint32_t clock = (uart == UART_PERIPHERAL_1) ? SystemCoreClock : (SystemCoreClock / 2);
    uint32_t divider = clock / baudrate;
    uint32_t mantissa = divider >> 4;
    uint32_t fraction = divider & 0x0F;
    return (mantissa << 4) | fraction;
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
        RCC_APB2ENR |= RCC_UART1_EN;
    } else if (uart == UART_PERIPHERAL_2) {
        RCC_APB1ENR |= RCC_UART2_EN;
    } else {
        RCC_APB1ENR |= RCC_UART3_EN;
    }
}

// =============================================================================
// API Implementation
// =============================================================================
void uartInit(UART_Peripheral_t uart, UART_Baudrate_t baudrate) {
    UART_TypeDef *uartx = uartGetBase(uart);

    /* Enable clock */
    uartEnableClock(uart);

    /* Configure pins */
    uartConfigPins(uart);

    /* Disable UART before configuration */
    uartx->CR1 = 0x0000;

    /* Set baudrate */
    uint32_t brr = uartCalcBaudrate(uart, (uint32_t)baudrate);
    uartx->BRR = brr;

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
