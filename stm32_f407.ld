/* STM32F4 Linker Script */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text*)
        *(.rodata*)
        *(.eh_frame*)
    } > FLASH

    _etext = LOADADDR(.text) + SIZEOF(.text);

    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    _sidata = LOADADDR(.data);

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    /DISCARD/ :
    {
        *(.gnu.lto_*)
        *(.gnu.attributes)
        *(.comment)
        *(.ARM.attributes)
    }
}

_estack = ORIGIN(RAM) + LENGTH(RAM);