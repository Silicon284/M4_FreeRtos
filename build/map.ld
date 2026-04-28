MEMORY
{
    FLASH : ORIGIN = 0x08100000, LENGTH = 1024K
    RAM : ORIGIN = 0x10000000, LENGTH = 288K
}

/* Define stack and heap sizes */
STACK_SIZE = 0x1000;   /* 2KB main stack */

SECTIONS
{
    .text : {
        _stext = .;
        *(.vectors*)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data : ALIGN(4) {
        _sdata = .;
        _sidata = LOADADDR(.data);
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    .bss : ALIGN (4) {
        _sbss = .;
        *(.bss*)
        *(COMMON)    /* ← This ensures COMMON variables go in BSS */
        . = ALIGN(4);
        _ebss = .;
        end = .;     /* Define end symbol for _sbrk */
        _end = .;    /* Alternative symbol name */
    } > RAM


    __StackTop = ORIGIN(RAM) + LENGTH(RAM);



    /* Reserve space for stack */
   /* .stack : ALIGN(8) {
        _stack_start = .;
        . = . + STACK_SIZE;
        _stack_end = .;
        __StackTop = .;
    } > RAM */

    PROVIDE(__stack = __StackTop - 4);
}