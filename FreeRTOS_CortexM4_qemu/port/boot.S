    .section .vectors
vector_table:
    .word __stack               @ 0: Initial stack pointer (end of RAM)
    .word reset_handler         @ 1: Reset handler address
    .word NMI_Handler           @ 2: Non-Maskable Interrupt
    .word HardFault_Handler     @ 3: Hard Fault
    .word MemManage_Handler     @ 4: Memory Management Fault
    .word BusFault_Handler      @ 5: Bus Fault
    .word UsageFault_Handler    @ 6: Usage Fault
    .word Default_Handler       @ 7: Reserved
    .word Default_Handler       @ 8: Reserved
    .word Default_Handler       @ 9: Reserved
    .word Default_Handler       @ 10: Reserved
    .word vPortSVCHandler       @ 11: SVCall (FreeRTOS)
    .word DebugMon_Handler      @ 12: Debug Monitor
    .word Default_Handler       @ 13: Reserved
    .word xPortPendSVHandler    @ 14: PendSV (FreeRTOS)
    .word xPortSysTickHandler   @ 15: SysTick (FreeRTOS)
    
    .space 100

    .section .text
    .align 2
    .global reset_handler
    .thumb_func
reset_handler:
    b start
    b .

    .section .text
    .align 1
    .type Default_Handler, %function
Default_Handler:
    b .

    @ Weak aliases for exception handlers
    .weak NMI_Handler
    .thumb_set NMI_Handler, Default_Handler
    
    .weak HardFault_Handler
    .thumb_set HardFault_Handler, Default_Handler
    
    .weak MemManage_Handler
    .thumb_set MemManage_Handler, Default_Handler
    
    .weak BusFault_Handler
    .thumb_set BusFault_Handler, Default_Handler
    
    .weak UsageFault_Handler
    .thumb_set UsageFault_Handler, Default_Handler
    
    .weak DebugMon_Handler
    .thumb_set DebugMon_Handler, Default_Handler