/**
 * @file main.c
 * @brief CustomRTOS Application - STM32F1 Air Quality Monitoring
 *
 * Chức năng: Đọc cảm biến chất lượng không khí và gửi lên ESP32
 * Cảm biến: MQ2 (khí gas), DHT11 (nhiệt độ/độ ẩm)
 * Chu kỳ: Đọc và gửi mỗi 2 giây
 */

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

/**
 * @brief Entry point - được gọi từ startup.c Reset_Handler
 */
int main(void) {
    /* ========== Hardware Initialization ========== */

    /* Khởi tạo system clock (72MHz) và GPIO clocks */
    systemInit();

    /* Khởi tạo SysTick cho ngắt 1ms (dùng cho getSystemTick()) */
    sysTickInit();

#ifdef STM32F1
    /* Cấu hình LED trên board (PC13 - active LOW) */
    gpioInitPin(GPIO_PORT_C, 13, GPIO_MODE_OUTPUT_50MHZ, GPIO_CNF_PUSHPULL);
    gpioWritePin(GPIO_PORT_C, 13, 1);  /* LED off (1 = OFF với active-low) */

    /* Khởi tạo cảm biến: MQ2 (ADC) + DHT11 (1-wire) */
    airQualityInit();

    /* Khởi tạo ESP32 WiFi module (UART1 @ 115200 baud) */
    esp01Init();

#elif defined(STM32F4)
    /* Cấu hình LED trên STM32F4 Discovery (PD12) */
    gpioInitPinSimple(GPIO_PORT_D, 12, GPIO_MODE_OUTPUT);
    gpioWritePin(GPIO_PORT_D, 12, 0);  /* LED off */

    /* Khởi tạo ADC cho MQ2 sensor */
    adcInit();

    /* Khởi tạo ESP32 WiFi module */
    esp01Init();
#endif

    /* ========== Main Loop ========== */

    uint32_t lastSend = 0;           /* Thời điểm gửi cuối */
    AirQuality_Data_t sensorData;    /* Buffer lưu dữ liệu cảm biến */

    while (1) {
        uint32_t now = getSystemTick();  /* Lấy thời gian hiện tại (ms) */

        /* Đọc và gửi mỗi 2000ms (2 giây) */
        if (now - lastSend >= 2000) {
            lastSend = now;

#ifdef STM32F1
            /* Bật LED báo hiệu đang đọc (active-low: 0 = ON) */
            gpioWritePin(GPIO_PORT_C, 13, 0);

            /* Đọc tất cả cảm biến (MQ2 + DHT11) */
            airQualityRead(&sensorData);

            /* Gửi dữ liệu lên ESP32
             * Format: "[T,H,MQ2]\n" VD: "[25,65,2048]\n"
             * Nếu DHT11 lỗi, gửi -1 cho T và H */
            if (sensorData.dht11_error == 0) {
                esp01SendReading(sensorData.temperature, sensorData.humidity, sensorData.mq2_adc);
            } else {
                esp01SendReading(-1, -1, sensorData.mq2_adc);
            }

            /* Tắt LED */
            gpioWritePin(GPIO_PORT_C, 13, 1);

#elif defined(STM32F4)
            /* Bật LED (active-high: 1 = ON) */
            gpioWritePin(GPIO_PORT_D, 12, 1);

            /* Đọc ADC và gửi lên ESP32 */
            uint16_t adcValue = adcRead();
            esp01SendReading(adcValue);

            /* Tắt LED */
            gpioWritePin(GPIO_PORT_D, 12, 0);
#endif
        }
    }

    return 0;
}
