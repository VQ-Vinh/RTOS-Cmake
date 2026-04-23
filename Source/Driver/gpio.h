/**
 * @file gpio.h
 * @brief GPIO Driver - Control GPIO pins
 *
 * Chức năng: Điều khiển chân GPIO (vào/ra, đọc/ghi)
 * Hỗ trợ: Port A, B, C | Pin 0-15
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/* ========== Chế độ hoạt động ========== */
typedef enum {
    GPIO_MODE_INPUT     = 0x0,      /* Input mode */
    GPIO_MODE_OUTPUT_10MHZ = 0x1,  /* Output 10MHz */
    GPIO_MODE_OUTPUT_2MHZ  = 0x2, /* Output 2MHz */
    GPIO_MODE_OUTPUT_50MHZ = 0x3  /* Output 50MHz */
} GPIO_Mode_t;

/* ========== Cấu hình chân ========== */
typedef enum {
    /* Input modes */
    GPIO_CNF_ANALOG       = 0x0,  /* Analog mode */
    GPIO_CNF_FLOATING     = 0x1,  /* Floating input */
    GPIO_CNF_PUPD         = 0x2,  /* Pull-up/Pull-down */

    /* Output modes */
    GPIO_CNF_PUSHPULL     = 0x0,  /* Push-pull output */
    GPIO_CNF_OPENDRAIN    = 0x1,  /* Open-drain output */
    GPIO_CNF_ALT_PUSHPULL = 0x2,  /* Alternate push-pull */
    GPIO_CNF_ALT_OPENDRAIN = 0x3  /* Alternate open-drain */
} GPIO_Cnf_t;

/* ========== Định nghĩa Port ========== */
typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2
} GPIO_Port_t;

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo chân GPIO
 * @param port: Port (A/B/C)
 * @param pin: Pin (0-15)
 * @param mode: Chế độ (INPUT/OUTPUT)
 * @param cnf: Cấu hình (PUSHPULL/FLOATING/...)
 */
void gpioInitPin(GPIO_Port_t port, uint8_t pin, GPIO_Mode_t mode, GPIO_Cnf_t cnf);

/**
 * @brief Ghi giá trị ra chân GPIO
 * @param port: Port
 * @param pin: Pin
 * @param state: 0 = LOW, non-zero = HIGH
 */
void gpioWritePin(GPIO_Port_t port, uint8_t pin, uint8_t state);

/**
 * @brief Đảo trạng thái chân GPIO
 * @param port: Port
 * @param pin: Pin
 */
void gpioTogglePin(GPIO_Port_t port, uint8_t pin);

/**
 * @brief Đọc giá trị từ chân GPIO
 * @param port: Port
 * @param pin: Pin
 * @return: 0 = LOW, 1 = HIGH
 */
uint8_t gpioReadPin(GPIO_Port_t port, uint8_t pin);

#endif /* GPIO_H */
