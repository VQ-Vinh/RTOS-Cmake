// STM32F103 Startup code

#include <stdint.h>

extern uint32_t _estack;
extern void main(void);
extern void SysTick_Handler(void);

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
    while(1);
}

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void) {
    // Copy initialized data from FLASH to RAM
    for(uint32_t *src = &_sidata, *dst = &_sdata; dst < &_edata; src++, dst++) {
        *dst = *src;
    }
    
    // Zero out BSS section
    for(uint32_t *dst = &_sbss; dst < &_ebss; dst++) {
        *dst = 0;
    }
    
    // Call main
    main();
    
    // Loop forever
    while(1);
}

// Vector table
typedef void (*pFunc)(void);

__attribute__ ((used, section(".isr_vector"))) const pFunc VectorTable[] = {
    (pFunc)&_estack,            // 0x00 Stack pointer
    Reset_Handler,              // 0x04 Reset
    NMI_Handler,                // 0x08 NMI
    HardFault_Handler,          // 0x0C Hard Fault
    MemManage_Handler,          // 0x10 Mem Manage
    BusFault_Handler,           // 0x14 Bus Fault
    UsageFault_Handler,         // 0x18 Usage Fault
    0,                          // 0x1C Reserved
    0,                          // 0x20 Reserved
    0,                          // 0x24 Reserved
    0,                          // 0x28 Reserved
    SVC_Handler,                // 0x2C SVCall
    DebugMon_Handler,           // 0x30 Debug Monitor
    0,                          // 0x34 Reserved
    PendSV_Handler,             // 0x38 PendSV
    SysTick_Handler,            // 0x3C SysTick (position 15)
};
