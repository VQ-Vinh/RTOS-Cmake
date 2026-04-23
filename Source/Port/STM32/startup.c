/**
 * @file startup.c
 * @brief STM32F1 Startup Code - Entry point sau reset
 *
 * Chức năng:
 * 1. Vector table đặt tại .isr_vector section
 * 2. Reset_Handler: copy data, zero BSS, gọi main()
 * 3. Default handlers cho các exception/interrupt
 */

#include <stdint.h>

/* ========== External references ========== */

extern uint32_t _estack;        /* Stack pointer từ linker script */
extern void main(void);         /* Entry point của ứng dụng */
extern void SysTick_Handler(void);

/* ========== Exception Handlers ========== */

void Reset_Handler(void);
void NMI_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));
void HardFault_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));
void MemManage_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__ ((weak, alias ("Default_Handler")));

void Default_Handler(void) {
    while(1);  /* Loop vô tận nếu có exception không xử lý */
}

/* ========== External data sections (linker script) ========== */

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss;

/**
 * @brief Reset Handler - Được gọi khi MCU reset
 *
 * Thứ tự:
 * 1. Copy .data from FLASH to RAM
 * 2. Zero .bss section
 * 3. Gọi main()
 * 4. Loop forever (nếu main return)
 */
void Reset_Handler(void) {
    /* Copy initialized data from FLASH to RAM */
    for(uint32_t *src = &_sidata, *dst = &_sdata; dst < &_edata; src++, dst++) {
        *dst = *src;
    }

    /* Zero out BSS section */
    for(uint32_t *dst = &_sbss; dst < &_ebss; dst++) {
        *dst = 0;
    }

    /* Gọi main */
    main();

    /* Loop forever nếu main return */
    while(1);
}

/* ========== Vector Table ========== */

typedef void (*pFunc)(void);

/**
 * @brief Vector Table - Địa chỉ các interrupt handlers
 *
 * Đặt tại .isr_vector section ( linker script sẽ put ở 0x08000000)
 */
__attribute__ ((used, section(".isr_vector"))) const pFunc VectorTable[] = {
    (pFunc)&_estack,            /* 0x00 Stack pointer */
    Reset_Handler,               /* 0x04 Reset */
    NMI_Handler,                 /* 0x08 NMI */
    HardFault_Handler,           /* 0x0C Hard Fault */
    MemManage_Handler,           /* 0x10 Mem Manage */
    BusFault_Handler,            /* 0x14 Bus Fault */
    UsageFault_Handler,          /* 0x18 Usage Fault */
    0,                           /* 0x1C Reserved */
    0,                           /* 0x20 Reserved */
    0,                           /* 0x24 Reserved */
    0,                           /* 0x28 Reserved */
    SVC_Handler,                 /* 0x2C SVCall */
    DebugMon_Handler,            /* 0x30 Debug Monitor */
    0,                           /* 0x34 Reserved */
    PendSV_Handler,              /* 0x38 PendSV */
    SysTick_Handler,             /* 0x3C SysTick (position 15) */
};
