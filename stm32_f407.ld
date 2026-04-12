/* Linker script for STM32F407VG Cortex-M4 ARM MCU */

MEMORY
{
    flash (rx): ORIGIN = 0x08000000, LENGTH = 1024K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } >flash

    .text : {
        KEEP(*(.startup))
        *(.text*)
        *(.rodata*)
        *(.eh_frame*)
    } >flash

    _etext = LOADADDR(.text) + SIZEOF(.text);

    .data : {
        _sdata = .;
        *(.data*)
        _edata = .;
    } >ram AT > flash

    _sidata = LOADADDR(.data);

    .bss : {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } >ram

    /DISCARD/ : {
        *(.gnu.lto_*)
        *(.gnu.attributes)
        *(.comment)
        *(.ARM.attributes)
    }
}

_estack = ORIGIN(ram) + LENGTH(ram);