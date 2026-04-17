#include <stdint.h>
#include "adc.h"
#include "esp01.h"
#include "port.h"
#include "gpio.h"

int main(void) {
    // Initialize system hardware (clocks, RCC, GPIO)
    systemInit();

    // Initialize SysTick for 1ms tick
    sysTickInit();

    // Configure PC13 as output (LED on STM32F103C8T6)
    gpioInitPin(GPIO_PORT_C, 13, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
    gpioWritePin(GPIO_PORT_C, 13, 1);  // LED off (active-low)

    // Initialize ADC (PB0 - channel 8 for MQ2 gas sensor)
    adcInit();

    // Initialize ESP32 connectivity (UART1: PA9=TX, PA10=RX)
    esp01Init();

    uint32_t lastTick = 0;

    while (1) {
        uint32_t now = getSystemTick();

        // Read sensor and send data every 1000ms
        if (now - lastTick >= 1000) {
            lastTick = now;
            gpioTogglePin(GPIO_PORT_C, 13);  // Toggle LED indicator

            // Read MQ2 ADC value (0-4095)
            uint16_t mq2_value = adcRead();
            
            // Send to ESP32: "MQ2:XXXX\n"
            esp01SendReading(mq2_value);
        }
    }

    return 0;
}