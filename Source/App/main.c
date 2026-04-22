#include <stdint.h>
#include "port.h"

#ifdef STM32F1
#include "air_quality.h"
#include "gpio.h"
#include "uart.h"
#include "esp01.h"
#include "dht11.h"
#include "mq2.h"
#elif defined(STM32F4)
#include "adc_f4.h"
#include "esp01_f4.h"
#include "gpio.h"
#endif

int main(void) {
    // Initialize system hardware (clocks, RCC, GPIO)
    systemInit();

    // Initialize SysTick for 1ms tick
    sysTickInit();

#ifdef STM32F1
    // Configure PC13 as output (LED on STM32F103C8T6)
    gpioInitPin(GPIO_PORT_C, 13, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
    gpioWritePin(GPIO_PORT_C, 13, 1);  // LED off (active-low)

    // Initialize air quality sensors (MQ2 + DHT11)
    airQualityInit();

    // Initialize ESP32 connectivity (UART1: PA9=TX, PA10=RX)
    esp01Init();

#elif defined(STM32F4)
    // Configure PD12 as output (Green LED on STM32F4 Discovery)
    gpioInitPinSimple(GPIO_PORT_D, 12, GPIO_MODE_OUTPUT);
    gpioWritePin(GPIO_PORT_D, 12, 0);  // LED off
    // Initialize ADC (PA0 - channel 0 for MQ2 gas sensor)
    adcInit();
    // Initialize ESP32 connectivity (UART1: PA9=TX, PA10=RX)
    esp01Init();
#endif

    uint32_t lastSend = 0;
    AirQuality_Data_t sensorData;

    while (1) {
        uint32_t now = getSystemTick();

        // Read sensors and send every 2000ms (2 seconds)
        if (now - lastSend >= 2000) {
            lastSend = now;

#ifdef STM32F1
            // LED on (active-low: 0 = on)
            gpioWritePin(GPIO_PORT_C, 13, 0);

            // Read all air quality sensors (MQ2 + DHT11)
            airQualityRead(&sensorData);

            // Send combined data: [temperature, humidity, mq2_adc]
            if (sensorData.dht11_error == 0) {
                esp01SendReading(sensorData.temperature, sensorData.humidity, sensorData.mq2_adc);
            } else {
                // DHT11 error - send with error indicator
                esp01SendReading(-1, -1, sensorData.mq2_adc);
            }

            // LED off immediately
            gpioWritePin(GPIO_PORT_C, 13, 1);
#elif defined(STM32F4)
            // LED on (active-high: 1 = on)
            gpioWritePin(GPIO_PORT_D, 12, 1);
            // Read and send
            uint16_t adcValue = adcRead();
            esp01SendReading(adcValue);
            // LED off immediately
            gpioWritePin(GPIO_PORT_D, 12, 0);
#endif
        }
    }

    return 0;
}
