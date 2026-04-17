#include <stdint.h>
#include "adc.h"
#include "esp01.h"
#include "port.h"
#include "gpio.h"
#include "uart.h"

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

    uint32_t lastToggle = 0;
    uint32_t lastSend = 0;
    uint32_t ledOffTime = 0;

    while (1) {
        uint32_t now = getSystemTick();

        // Toggle LED every 500ms as indicator
        if (now - lastToggle >= 500) {
            lastToggle = now;
            gpioTogglePin(GPIO_PORT_C, 13);
        }

        // Read ADC and send every 100ms
        if (now - lastSend >= 100) {
            lastSend = now;

            // LED on for 200ms when sending
            gpioWritePin(GPIO_PORT_C, 13, 0);  // LED on (active-low)
            ledOffTime = now + 200;

            // Read ADC value and send immediately
            uint16_t adcValue = adcRead();
            esp01SendReading(adcValue);
        }

        // Turn LED off after 200ms
        if (ledOffTime > 0 && now >= ledOffTime) {
            gpioWritePin(GPIO_PORT_C, 13, 1);  // LED off
            ledOffTime = 0;
        }
    }

    return 0;
}