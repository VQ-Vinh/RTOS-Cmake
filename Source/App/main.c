#include <stdint.h>
#include "port.h"

#ifdef STM32F1
#include "adc.h"
#include "esp01.h"
#include "gpio.h"
#include "uart.h"
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
    // Initialize ADC (PB0 - channel 8 for MQ2 gas sensor)
    adcInit();
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

    while (1) {
        uint32_t now = getSystemTick();

        // Read ADC and send every 200ms
        if (now - lastSend >= 200) {
            lastSend = now;

#ifdef STM32F1
            // LED on (active-low: 0 = on)
            gpioWritePin(GPIO_PORT_C, 13, 0);
            // Read and send
            uint16_t adcValue = adcRead();
            esp01SendReading(adcValue);
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
