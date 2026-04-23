/**
 * @file gpio.c
 * @brief GPIO Driver Implementation
 */

#include "gpio.h"
#include "rcc.h"

/* ========== Địa chỉ cơ sở ========== */
#define GPIOA_BASE      0x40010800
#define GPIOB_BASE      0x40010C00
#define GPIOC_BASE      0x40011000

/* ========== Thanh ghi GPIO ========== */
typedef struct {
    volatile uint32_t CRL;      /* Config low - pins 0-7 */
    volatile uint32_t CRH;      /* Config high - pins 8-15 */
    volatile uint32_t IDR;      /* Input data */
    volatile uint32_t ODR;      /* Output data */
    volatile uint32_t BSRR;     /* Bit set/reset */
    volatile uint32_t BRR;      /* Bit reset */
    volatile uint32_t LCKR;     /* Lock */
} GPIO_Reg_t;

#define GPIOA   ((GPIO_Reg_t *)GPIOA_BASE)
#define GPIOB   ((GPIO_Reg_t *)GPIOB_BASE)
#define GPIOC   ((GPIO_Reg_t *)GPIOC_BASE)

/* Bit enable clock cho từng port */
#define RCC_IOPAEN     (1 << 2)
#define RCC_IOPBEN     (1 << 3)
#define RCC_IOPCEN     (1 << 4)

/* Bảng tra cứu port và clock */
static GPIO_Reg_t* const gpioPorts[] = {GPIOA, GPIOB, GPIOC};
static const uint32_t rccClocks[] = {RCC_IOPAEN, RCC_IOPBEN, RCC_IOPCEN};

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo chân GPIO
 *
 * Mỗi chân cần 4 bits config: MODE(2 bits) + CNF(2 bits)
 * Với pins 0-7 dùng CRL, pins 8-15 dùng CRH
 */
void gpioInitPin(GPIO_Port_t port, uint8_t pin, GPIO_Mode_t mode, GPIO_Cnf_t cnf) {
    /* Bật clock cho port */
    rccEnableClock(rccClocks[port]);

    GPIO_Reg_t *gpio = gpioPorts[port];
    /* Chọn CRL cho pin 0-7, CRH cho pin 8-15 */
    volatile uint32_t *cr = (pin < 8) ? &gpio->CRL : &gpio->CRH;
    /* Mỗi pin dùng 4 bits trong thanh ghi config */
    uint8_t shift = (pin % 8) * 4;

    /* Xóa config cũ và set config mới */
    *cr &= ~(0xF << shift);
    *cr |= ((cnf << 2) | mode) << shift;
}

/**
 * @brief Ghi giá trị ra chân GPIO (dùng ODR)
 */
void gpioWritePin(GPIO_Port_t port, uint8_t pin, uint8_t state) {
    GPIO_Reg_t *gpio = gpioPorts[port];
    if (state) {
        gpio->ODR |= (1 << pin);      /* Set bit = HIGH */
    } else {
        gpio->ODR &= ~(1 << pin);     /* Clear bit = LOW */
    }
}

/**
 * @brief Đảo trạng thái chân GPIO
 */
void gpioTogglePin(GPIO_Port_t port, uint8_t pin) {
    GPIO_Reg_t *gpio = gpioPorts[port];
    gpio->ODR ^= (1 << pin);          /* XOR để đảo bit */
}

/**
 * @brief Đọc giá trị từ chân GPIO (dùng IDR)
 */
uint8_t gpioReadPin(GPIO_Port_t port, uint8_t pin) {
    GPIO_Reg_t *gpio = gpioPorts[port];
    return (gpio->IDR >> pin) & 0x1;  /* Đọc bit từ IDR */
}
