/**
 * @file dht11.c
 * @brief DHT11 Temperature & Humidity Sensor Driver Implementation
 * @note Single-wire communication protocol for DHT11
 * @note Timing-critical: disables interrupts during data read
 */

#include "dht11.h"
#include "../Driver/gpio.h"
#include "../Driver/systick.h"
#include <stddef.h>

/* DHT11 protocol timing constants */
#define DHT11_BIT_COUNT       40
#define DHT11_TIMEOUT_US      100
#define DHT11_START_LOW_MS    20
#define DHT11_START_HIGH_US   40

/* DHT11 threshold for bit recognition (in microseconds) */
/* HIGH duration > 50us = 1, otherwise 0 */
#define DHT11_BIT1_THRESHOLD_US  50

/* Internal function prototypes */
static void DHT11_SetPinOutput(DHT11_Handle_t *handle);
static void DHT11_SetPinInput(DHT11_Handle_t *handle);
static uint8_t DHT11_ReadPin(DHT11_Handle_t *handle);
static void DHT11_DelayUs(uint32_t us);
static DHT11_StatusTypeDef DHT11_WaitForState(DHT11_Handle_t *handle, uint8_t state, uint32_t timeout_us);

/* Error messages */
const char* const DHT11_ErrorMsg[__DHT11_STATUS_TYPEDEF_COUNT__] = {
    [DHT11_OK]               = "OK",
    [DHT11_TIMEOUT]          = "TIMEOUT",
    [DHT11_ERROR]            = "ERROR",
    [DHT11_CHECKSUM_MISMATCH] = "CHECKSUM MISMATCH",
    [DHT11_INVALID_PARAMETER]= "INVALID PARAMETER"
};

/**
 * @brief Set pin as output mode
 */
static void DHT11_SetPinOutput(DHT11_Handle_t *handle) {
    gpioInitPin(handle->port, handle->pin, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
}

/**
 * @brief Set pin as input mode (floating)
 */
static void DHT11_SetPinInput(DHT11_Handle_t *handle) {
    gpioInitPin(handle->port, handle->pin, GPIO_MODE_INPUT, GPIO_CNF_FLOATING);
}

/**
 * @brief Read current pin state
 */
static uint8_t DHT11_ReadPin(DHT11_Handle_t *handle) {
    return gpioReadPin(handle->port, handle->pin);
}

/**
 * @brief Blocking microsecond delay using SysTick
 */
static void DHT11_DelayUs(uint32_t us) {
    systickDelayUs(us);
}

/**
 * @brief Wait for pin to reach specific state with timeout
 * @param state: 0 = low, 1 = high
 * @param timeout_us: timeout in microseconds
 * @return DHT11 status
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

void DHT11_Init(DHT11_Handle_t *handle, uint8_t port, uint8_t pin) {
    if (handle == NULL) {
        return;
    }

    handle->port = port;
    handle->pin = pin;
    handle->Temperature = 0.0f;
    handle->Humidity = 0.0f;
    handle->Status = DHT11_OK;

    /* Set pin as output, high (idle state) */
    DHT11_SetPinOutput(handle);
    gpioWritePin(handle->port, handle->pin, 1);

    /* Wait for DHT11 to stabilize (~1 second) */
    systickDelayMs(1000);
}

void DHT11_DeInit(DHT11_Handle_t *handle) {
    if (handle == NULL) {
        return;
    }

    /* Set pin as input floating */
    DHT11_SetPinInput(handle);
}

const char* DHT11_GetErrorMsg(DHT11_StatusTypeDef status) {
    if (status >= __DHT11_STATUS_TYPEDEF_COUNT__) {
        return DHT11_ErrorMsg[DHT11_INVALID_PARAMETER];
    }
    return DHT11_ErrorMsg[status];
}

DHT11_StatusTypeDef DHT11_ReadData(DHT11_Handle_t *handle) {
    uint8_t dht11_data[5] = {0};
    uint8_t bit_index = 0;
    DHT11_StatusTypeDef status;

    if (handle == NULL) {
        return DHT11_INVALID_PARAMETER;
    }

    /* Disable interrupts for timing-critical section */
    __asm__("CPSID I");

    /* === Step 1: Send start signal === */
    /* MCU pulls low for >= 18ms */
    DHT11_SetPinOutput(handle);
    gpioWritePin(handle->port, handle->pin, 0);
    systickDelayMs(DHT11_START_LOW_MS);

    /* MCU pulls high for 40us then switch to input */
    gpioWritePin(handle->port, handle->pin, 1);
    DHT11_DelayUs(DHT11_START_HIGH_US);
    DHT11_SetPinInput(handle);

    /* === Step 2: DHT11 response signal === */
    /* DHT11 pulls low for 80us, then high for 80us */

    /* Wait for DHT11 pulling low (response start) */
    status = DHT11_WaitForState(handle, 0, DHT11_TIMEOUT_US * 10);
    if (status != DHT11_OK) {
        __asm__("CPSIE I");
        handle->Status = status;
        return status;
    }

    /* Wait for DHT11 pulling high (response end) */
    status = DHT11_WaitForState(handle, 1, DHT11_TIMEOUT_US * 10);
    if (status != DHT11_OK) {
        __asm__("CPSIE I");
        handle->Status = status;
        return status;
    }

    /* === Step 3: Read 40 bits of data === */
    for (bit_index = 0; bit_index < DHT11_BIT_COUNT; bit_index++) {
        /* Wait for start of bit (low pulse) */
        status = DHT11_WaitForState(handle, 0, DHT11_TIMEOUT_US * 10);
        if (status != DHT11_OK) {
            __asm__("CPSIE I");
            handle->Status = status;
            return status;
        }

        /* Measure high pulse duration */
        volatile uint32_t start = systickGetValue();
        uint32_t ticks_per_us = SystemCoreClock / 1000000;

        /* Wait for pin to go low */
        while (DHT11_ReadPin(handle) == 1) {
            uint32_t elapsed = start - systickGetValue();
            if (elapsed >= DHT11_TIMEOUT_US * ticks_per_us * 10) {
                break;
            }
        }

        /* Calculate high time in microseconds */
        uint32_t high_time_us = (start - systickGetValue()) / ticks_per_us;

        /* Decode bit: if high time > 50us, bit is 1 */
        if (high_time_us > DHT11_BIT1_THRESHOLD_US) {
            dht11_data[bit_index / 8] |= (1 << (7 - (bit_index % 8)));
        }

        /* Wait for bit to end (pin goes low) with timeout */
        volatile uint32_t bit_start = systickGetValue();
        while (DHT11_ReadPin(handle) == 1) {
            uint32_t elapsed = bit_start - systickGetValue();
            if (elapsed >= DHT11_TIMEOUT_US * ticks_per_us * 10) {
                break;
            }
        }
    }

    /* Re-enable interrupts */
    __asm__("CPSIE I");

    /* === Step 4: Verify checksum === */
    uint8_t checksum = dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3];
    if (checksum != dht11_data[4]) {
        handle->Status = DHT11_CHECKSUM_MISMATCH;
        return DHT11_CHECKSUM_MISMATCH;
    }

    /* Update handle with readings */
    handle->Humidity = (float)dht11_data[0] + ((float)dht11_data[1] * 0.1f);
    handle->Temperature = (float)dht11_data[2] + ((float)dht11_data[3] * 0.1f);

    /* Return pin to idle state */
    DHT11_SetPinOutput(handle);
    gpioWritePin(handle->port, handle->pin, 1);

    handle->Status = DHT11_OK;
    return DHT11_OK;
}

float DHT11_ReadTemperatureC(DHT11_Handle_t *handle) {
    DHT11_ReadData(handle);
    return handle->Temperature;
}

float DHT11_ReadTemperatureF(DHT11_Handle_t *handle) {
    float tempC = DHT11_ReadTemperatureC(handle);
    return (tempC * 1.8f) + 32.0f;
}

float DHT11_ReadHumidity(DHT11_Handle_t *handle) {
    DHT11_ReadData(handle);
    return handle->Humidity;
}