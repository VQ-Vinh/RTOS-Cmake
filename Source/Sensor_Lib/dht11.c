/**
 * @file dht11.c
 * @brief DHT11 Temperature & Humidity Sensor Driver Implementation
 * @note Single-wire communication protocol for DHT11
 */

#include "dht11.h"
#include "../Driver/gpio.h"
#include "../Driver/systick.h"
#include <stddef.h>

/* DHT11 data is 40 bits: 8 humidity integer + 8 humidity decimal + 8 temp integer + 8 temp decimal + 8 checksum */
#define DHT11_BIT_COUNT    40
#define DHT11_TIMEOUT_US  1000   // 1ms timeout for bit reading

/* Static variables for DHT11 pin configuration */
static GPIO_Port_t dht11_port;
static uint8_t dht11_pin;

/**
 * @brief Set pin as output (driving)
 */
static void dht11SetPinOutput(void) {
    gpioInitPin(dht11_port, dht11_pin, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
}

/**
 * @brief Set pin as input (floating)
 */
static void dht11SetPinInput(void) {
    gpioInitPin(dht11_port, dht11_pin, GPIO_MODE_INPUT, GPIO_CNF_FLOATING);
}

/**
 * @brief Read pin state
 */
static uint8_t dht11ReadPin(void) {
    return gpioReadPin(dht11_port, dht11_pin);
}

/**
 * @brief Set pin high
 */
static void dht11SetPinHigh(void) {
    gpioWritePin(dht11_port, dht11_pin, 1);
}

/**
 * @brief Set pin low
 */
static void dht11SetPinLow(void) {
    gpioWritePin(dht11_port, dht11_pin, 0);
}

void dht11Init(GPIO_Port_t port, uint8_t pin) {
    dht11_port = port;
    dht11_pin = pin;

    // Set pin as output initially
    dht11SetPinOutput();
    dht11SetPinHigh();  // Idle state (high)

    // Wait for DHT11 to stabilize (~1 second)
    systickDelayMs(1000);
}

int8_t dht11Read(DHT11_Data_t *data) {
    uint8_t dht11_data[5] = {0};
    uint8_t bit_index = 0;
    int8_t result = -1;

    if (data == NULL) {
        return -1;
    }

    // Step 1: Send start signal
    dht11SetPinOutput();
    dht11SetPinLow();   // Pull low for >= 18ms
    systickDelayMs(20); // Use existing delay function for longer wait
    dht11SetPinHigh();  // Pull high

    systickDelayUs(30); // Wait 30us (pull high, DHT11 will pull low)

    // Step 2: DHT11 response signal
    dht11SetPinInput();

    // Wait for DHT11 to pull low (response start)
    volatile uint32_t timeout = DHT11_TIMEOUT_US * 50;
    while (dht11ReadPin() == 1) {
        if (timeout-- == 0) {
            return -1;  // Timeout waiting for low
        }
    }

    // Wait for DHT11 to release (go high)
    timeout = DHT11_TIMEOUT_US * 50;
    while (dht11ReadPin() == 0) {
        if (timeout-- == 0) {
            return -1;  // Timeout waiting for high
        }
    }

    // Step 3: Read 40 bits of data
    for (bit_index = 0; bit_index < DHT11_BIT_COUNT; bit_index++) {
        // Wait for bit start (low pulse)
        timeout = DHT11_TIMEOUT_US * 50;
        while (dht11ReadPin() == 1) {
            if (timeout-- == 0) {
                return -1;
            }
        }

        // Measure bit duration
        timeout = DHT11_TIMEOUT_US * 50;
        while (dht11ReadPin() == 0) {
            if (timeout-- == 0) {
                return -1;
            }
        }

        // Wait ~30us then check pin state (data bit)
        systickDelayUs(40);

        // If pin is high after ~40us, bit is 1
        if (dht11ReadPin() == 1) {
            dht11_data[bit_index / 8] |= (1 << (7 - (bit_index % 8)));
        }

        // Wait for bit end
        timeout = DHT11_TIMEOUT_US * 50;
        while (dht11ReadPin() == 1) {
            if (timeout-- == 0) {
                break;
            }
        }
    }

    // Step 4: Verify checksum
    data->humidity = dht11_data[0];      // Humidity integer
    data->temperature = dht11_data[2];    // Temperature integer
    data->checksum = dht11_data[4];

    uint8_t expected_checksum = dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3];
    if (expected_checksum == data->checksum) {
        result = 0;  // Success
    } else {
        result = -1;  // Checksum error
    }

    // Return pin to idle state
    dht11SetPinOutput();
    dht11SetPinHigh();

    return result;
}
