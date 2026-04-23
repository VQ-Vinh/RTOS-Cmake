/**
 * @file dht11.c
 * @brief DHT11 Driver Implementation
 *
 * Giao tiếp 1-wire với DHT11
 * Timing-critical: disable interrupts khi đọc data
 */

#include "dht11.h"
#include "../Driver/gpio.h"
#include "../Driver/systick.h"
#include <stddef.h>

/* ========== Timing constants ========== */
#define DHT11_BIT_COUNT       40         /* 40 bits = 5 bytes */
#define DHT11_TIMEOUT_US      100        /* Timeout per bit (us) */
#define DHT11_START_LOW_MS    20         /* MCU pull low >= 18ms */
#define DHT11_START_HIGH_US   40        /* MCU pull high 20-40us */

/* Threshold để nhận bit: HIGH > 50us = 1 */
#define DHT11_BIT1_THRESHOLD_US  50

/* ========== Error messages ========== */
const char* const DHT11_ErrorMsg[__DHT11_STATUS_TYPEDEF_COUNT__] = {
    [DHT11_OK]               = "OK",
    [DHT11_TIMEOUT]          = "TIMEOUT",
    [DHT11_ERROR]            = "ERROR",
    [DHT11_CHECKSUM_MISMATCH] = "CHECKSUM MISMATCH",
    [DHT11_INVALID_PARAMETER]= "INVALID PARAMETER"
};

/* ========== Helper functions ========== */

/**
 * @brief Set pin mode output
 */
static void DHT11_SetPinOutput(DHT11_Handle_t *handle) {
    gpioInitPin(handle->port, handle->pin, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
}

/**
 * @brief Set pin mode input (floating)
 */
static void DHT11_SetPinInput(DHT11_Handle_t *handle) {
    gpioInitPin(handle->port, handle->pin, GPIO_MODE_INPUT, GPIO_CNF_FLOATING);
}

/**
 * @brief Đọc trạng thái pin
 */
static uint8_t DHT11_ReadPin(DHT11_Handle_t *handle) {
    return gpioReadPin(handle->port, handle->pin);
}

/**
 * @brief Delay microseconds
 */
static void DHT11_DelayUs(uint32_t us) {
    systickDelayUs(us);
}

/**
 * @brief Chờ pin đạt trạng thái mong muốn
 * @param state: 0 = LOW, 1 = HIGH
 * @param timeout_us: Timeout (microseconds)
 */
static DHT11_StatusTypeDef DHT11_WaitForState(DHT11_Handle_t *handle, uint8_t state, uint32_t timeout_us) {
    volatile uint32_t start = systickGetValue();
    uint32_t ticks_per_us = SystemCoreClock / 1000000;
    uint32_t timeout_ticks = timeout_us * ticks_per_us;

    while (DHT11_ReadPin(handle) != state) {
        uint32_t elapsed = start - systickGetValue();
        if (elapsed >= timeout_ticks) {
            return DHT11_TIMEOUT;
        }
    }
    return DHT11_OK;
}

/* ========== API Implementation ========== */

/**
 * @brief Khởi tạo DHT11
 *
 * - Set pin output, HIGH (idle state)
 * - Đợi 1 giây cho DHT11 ổn định
 */
void DHT11_Init(DHT11_Handle_t *handle, uint8_t port, uint8_t pin) {
    if (handle == NULL) return;

    handle->port = port;
    handle->pin = pin;
    handle->Temperature = 0.0f;
    handle->Humidity = 0.0f;
    handle->Status = DHT11_OK;

    /* Pin output, HIGH (idle) */
    DHT11_SetPinOutput(handle);
    gpioWritePin(handle->port, handle->pin, 1);

    /* Đợi DHT11 ổn định (~1 giây) */
    systickDelayMs(1000);
}

/**
 * @brief Tắt DHT11 - giải phóng pin
 */
void DHT11_DeInit(DHT11_Handle_t *handle) {
    if (handle == NULL) return;

    DHT11_SetPinInput(handle);
}

/**
 * @brief Lấy thông báo lỗi
 */
const char* DHT11_GetErrorMsg(DHT11_StatusTypeDef status) {
    if (status >= __DHT11_STATUS_TYPEDEF_COUNT__) {
        return DHT11_ErrorMsg[DHT11_INVALID_PARAMETER];
    }
    return DHT11_ErrorMsg[status];
}

/**
 * @brief Đọc dữ liệu từ DHT11
 *
 * Quy trình 1-wire:
 * 1. MCU gửi START: pull LOW >= 18ms, then HIGH 20-40us
 * 2. DHT11 response: pull LOW 80us, then HIGH 80us
 * 3. DHT11 gửi 40 bits data
 * 4. Verify checksum
 *
 * Lưu ý: Disable interrupts trong lúc đọc (timing critical)
 */
DHT11_StatusTypeDef DHT11_ReadData(DHT11_Handle_t *handle) {
    uint8_t dht11_data[5] = {0};
    uint8_t bit_index = 0;
    DHT11_StatusTypeDef status;

    if (handle == NULL) {
        return DHT11_INVALID_PARAMETER;
    }

    /* Disable interrupts cho timing chính xác */
    __asm__("CPSID I");

    /* === Bước 1: Gửi START signal === */
    /* MCU pull LOW >= 18ms */
    DHT11_SetPinOutput(handle);
    gpioWritePin(handle->port, handle->pin, 0);
    systickDelayMs(DHT11_START_LOW_MS);

    /* MCU pull HIGH 20-40us rồi chuyển sang input */
    gpioWritePin(handle->port, handle->pin, 1);
    DHT11_DelayUs(DHT11_START_HIGH_US);
    DHT11_SetPinInput(handle);

    /* === Bước 2: DHT11 response === */
    /* DHT11 pull LOW 80us, sau đó HIGH 80us */

    /* Chờ DHT11 pull LOW */
    status = DHT11_WaitForState(handle, 0, DHT11_TIMEOUT_US * 10);
    if (status != DHT11_OK) {
        __asm__("CPSIE I");
        handle->Status = status;
        return status;
    }

    /* Chờ DHT11 pull HIGH */
    status = DHT11_WaitForState(handle, 1, DHT11_TIMEOUT_US * 10);
    if (status != DHT11_OK) {
        __asm__("CPSIE I");
        handle->Status = status;
        return status;
    }

    /* === Bước 3: Đọc 40 bits === */
    for (bit_index = 0; bit_index < DHT11_BIT_COUNT; bit_index++) {
        /* Chờ start of bit (LOW pulse) */
        status = DHT11_WaitForState(handle, 0, DHT11_TIMEOUT_US * 10);
        if (status != DHT11_OK) {
            __asm__("CPSIE I");
            handle->Status = status;
            return status;
        }

        /* Measure HIGH duration */
        volatile uint32_t start = systickGetValue();
        uint32_t ticks_per_us = SystemCoreClock / 1000000;

        /* Chờ pin xuống LOW */
        while (DHT11_ReadPin(handle) == 1) {
            uint32_t elapsed = start - systickGetValue();
            if (elapsed >= DHT11_TIMEOUT_US * ticks_per_us * 10) {
                break;
            }
        }

        /* Tính thời gian HIGH (microseconds) */
        uint32_t high_time_us = (start - systickGetValue()) / ticks_per_us;

        /* Decode bit: HIGH > 50us = 1 */
        if (high_time_us > DHT11_BIT1_THRESHOLD_US) {
            dht11_data[bit_index / 8] |= (1 << (7 - (bit_index % 8)));
        }
    }

    /* Re-enable interrupts */
    __asm__("CPSIE I");

    /* === Bước 4: Verify checksum === */
    uint8_t checksum = dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3];
    if (checksum != dht11_data[4]) {
        handle->Status = DHT11_CHECKSUM_MISMATCH;
        return DHT11_CHECKSUM_MISMATCH;
    }

    /* Cập nhật handle với readings */
    handle->Humidity = (float)dht11_data[0] + ((float)dht11_data[1] * 0.1f);
    handle->Temperature = (float)dht11_data[2] + ((float)dht11_data[3] * 0.1f);

    /* Set pin lại idle state */
    DHT11_SetPinOutput(handle);
    gpioWritePin(handle->port, handle->pin, 1);

    handle->Status = DHT11_OK;
    return DHT11_OK;
}

/**
 * @brief Đọc nhiệt độ (Celsius)
 */
float DHT11_ReadTemperatureC(DHT11_Handle_t *handle) {
    DHT11_ReadData(handle);
    return handle->Temperature;
}

/**
 * @brief Đọc nhiệt độ (Fahrenheit)
 */
float DHT11_ReadTemperatureF(DHT11_Handle_t *handle) {
    float tempC = DHT11_ReadTemperatureC(handle);
    return (tempC * 1.8f) + 32.0f;
}

/**
 * @brief Đọc độ ẩm
 */
float DHT11_ReadHumidity(DHT11_Handle_t *handle) {
    DHT11_ReadData(handle);
    return handle->Humidity;
}
