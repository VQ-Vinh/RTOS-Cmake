/**
 * @file dht11.h
 * @brief DHT11 Temperature & Humidity Sensor Driver
 *
 * Chức năng: Đọc nhiệt độ và độ ẩm từ DHT11
 * Giao tiếp: Single-wire protocol (1-wire)
 */

#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

/* Forward declaration */
typedef struct _DHT11_Handle_t DHT11_Handle_t;

/* ========== Trạng thái DHT11 ========== */
typedef enum {
    DHT11_OK = 0,              /* Thành công */
    DHT11_TIMEOUT,             /* Timeout - DHT11 không phản hồi */
    DHT11_ERROR,               /* Lỗi tổng quát */
    DHT11_CHECKSUM_MISMATCH,    /* Checksum không khớp */
    DHT11_INVALID_PARAMETER,   /* Tham số không hợp lệ */
    __DHT11_STATUS_TYPEDEF_COUNT__
} DHT11_StatusTypeDef;

/* ========== Cấu trúc handle ========== */

/**
 * @brief DHT11 handle - lưu trữ thông tin giao tiếp
 */
typedef struct _DHT11_Handle_t {
    uint8_t port;               /* GPIO port (0=A, 1=B, 2=C) */
    uint8_t pin;                /* GPIO pin (0-15) */
    float Temperature;           /* Nhiệt độ (°C) */
    float Humidity;              /* Độ ẩm (%RH) */
    DHT11_StatusTypeDef Status; /* Trạng thái cuối operation */
} DHT11_Handle_t;

/* ========== Error messages ========== */
extern const char* const DHT11_ErrorMsg[__DHT11_STATUS_TYPEDEF_COUNT__];

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo DHT11
 * @param handle: DHT11 handle pointer
 * @param port: GPIO port (0=A, 1=B, 2=C)
 * @param pin: Pin number (0-15)
 *
 * Phải gọi trước mọi thao tác DHT11 khác
 */
void DHT11_Init(DHT11_Handle_t *handle, uint8_t port, uint8_t pin);

/**
 * @brief Tắt DHT11, giải phóng GPIO pin
 * @param handle: DHT11 handle pointer
 */
void DHT11_DeInit(DHT11_Handle_t *handle);

/**
 * @brief Lấy thông báo lỗi
 * @param status: Mã lỗi DHT11
 * @return: Chuỗi mô tả lỗi
 */
const char* DHT11_GetErrorMsg(DHT11_StatusTypeDef status);

/**
 * @brief Đọc dữ liệu từ DHT11
 * @param handle: DHT11 handle pointer
 * @return: Trạng thái DHT11 (DHT11_OK = thành công)
 *
 * Lưu ý:
 * - Blocking ~4ms
 * - Disable interrupts trong lúc đọc (đảm bảo timing)
 */
DHT11_StatusTypeDef DHT11_ReadData(DHT11_Handle_t *handle);

/**
 * @brief Đọc nhiệt độ (Celsius)
 * @param handle: DHT11 handle pointer
 * @return: Nhiệt độ (°C)
 */
float DHT11_ReadTemperatureC(DHT11_Handle_t *handle);

/**
 * @brief Đọc nhiệt độ (Fahrenheit)
 * @param handle: DHT11 handle pointer
 * @return: Nhiệt độ (°F)
 */
float DHT11_ReadTemperatureF(DHT11_Handle_t *handle);

/**
 * @brief Đọc độ ẩm
 * @param handle: DHT11 handle pointer
 * @return: Độ ẩm (%RH)
 */
float DHT11_ReadHumidity(DHT11_Handle_t *handle);

#endif /* DHT11_H */
