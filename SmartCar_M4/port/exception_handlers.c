#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "exception_handlers.h"

// Global variable to hold exception information for debugging
volatile ExceptionFrame_t g_exception_frame;

// Global variable to store hard fault debug information
volatile HardFaultDebugInfo_t g_hardFaultInfo = {0};


// Cortex-M3 Exception Handlers
void NMI_Handler(void)
{
    /* Non-Maskable Interrupt */
    BSP_LED_On(LED_RED); 
    while(1) {}
}

void HardFault_Handler(void)
{
    // Capture all fault status registers immediately
    g_hardFaultInfo.cfsr = *((volatile uint32_t*)0xE000ED28);  // CFSR
    g_hardFaultInfo.hfsr = *((volatile uint32_t*)0xE000ED2C);  // HFSR
    g_hardFaultInfo.mmfar = *((volatile uint32_t*)0xE000ED34); // MMFAR
    g_hardFaultInfo.bfar = *((volatile uint32_t*)0xE000ED38);  // BFAR
    g_hardFaultInfo.shcsr = *((volatile uint32_t*)0xE000ED24); // SHCSR
    
    // Get current stack pointer
    __asm volatile ("mov %0, sp" : "=r" (g_hardFaultInfo.stack_pointer));
    
    // Get FreeRTOS tick count (if scheduler is running)
    g_hardFaultInfo.tick_count = xTaskGetTickCount();
    
    // Get heap information (if heap is still functional)
    g_hardFaultInfo.heap_free = xPortGetFreeHeapSize();
    
    // Try to extract fault PC and LR from the stack (if stack is valid)
    if (g_hardFaultInfo.stack_pointer >= 0x30000000 && 
        g_hardFaultInfo.stack_pointer < 0x30002000) {
        
        uint32_t *stack = (uint32_t*)g_hardFaultInfo.stack_pointer;
        
        // Exception stack frame layout (if using MSP):
        // SP+0: R0, SP+4: R1, SP+8: R2, SP+12: R3
        // SP+16: R12, SP+20: LR, SP+24: PC, SP+28: xPSR
        g_hardFaultInfo.fault_r0 = stack[0];
        g_hardFaultInfo.fault_lr = stack[5];
        g_hardFaultInfo.fault_pc = stack[6];
    }
    
    // Analyze fault type
    g_hardFaultInfo.fault_type = FAULT_TYPE_UNKNOWN;
    
    if (g_hardFaultInfo.hfsr & SCB_HFSR_FORCED) {
        // This is an escalated fault - check CFSR for details
        if (g_hardFaultInfo.cfsr & 0xFF) {
            // MemManage fault bits (0-7)
            g_hardFaultInfo.fault_type = FAULT_TYPE_MEMMANAGE;
        } else if (g_hardFaultInfo.cfsr & 0xFF00) {
            // Bus fault bits (8-15)
            g_hardFaultInfo.fault_type = FAULT_TYPE_BUS_FAULT;
        } else if (g_hardFaultInfo.cfsr & 0xFFFF0000) {
            // Usage fault bits (16-31)
            g_hardFaultInfo.fault_type = FAULT_TYPE_USAGE_FAULT;
        } else {
            g_hardFaultInfo.fault_type = FAULT_TYPE_ESCALATED;
        }
    } else if (g_hardFaultInfo.hfsr & SCB_HFSR_VECTTBL) {
        g_hardFaultInfo.fault_type = FAULT_TYPE_VECTOR_TABLE;
    }
    
    // Check for potential stack overflow
    if (g_hardFaultInfo.stack_pointer < 0x30000100) {
        g_hardFaultInfo.fault_type = FAULT_TYPE_STACK_OVERFLOW;
    }
    
    // Check for heap corruption
    if (g_hardFaultInfo.heap_free == 0 || g_hardFaultInfo.heap_free > 0x2000) {
        g_hardFaultInfo.fault_type = FAULT_TYPE_HEAP_CORRUPTION;
    }
    
    /* ENHANCED BREAKPOINT: Now you have complete fault analysis */
    __asm volatile ("bkpt #99");
    
    /* If no debugger, just loop forever */
    while(1) {
        __asm volatile ("nop");
    }
}

void MemManage_Handler(void)
{
    /* Memory Management Fault - read fault address */
    volatile uint32_t cfsr = *((volatile uint32_t*)0xE000ED28);  // CFSR
    volatile uint32_t mmfar = *((volatile uint32_t*)0xE000ED34); // MMFAR
    (void)cfsr;  // Prevent optimization
    (void)mmfar;
    __asm("bkpt #0");
    for(;;);
}

void BusFault_Handler(void)
{
    /* Bus Fault - read fault address and status */
    volatile uint32_t cfsr = *((volatile uint32_t*)0xE000ED28);  // CFSR
    volatile uint32_t bfar = *((volatile uint32_t*)0xE000ED38);  // BFAR
    (void)cfsr;  // Prevent optimization
    (void)bfar;
    __asm("bkpt #0");
    for(;;);
}

void UsageFault_Handler(void)
{
    /* Usage Fault - undefined instruction, unaligned access, etc. */
    volatile uint32_t cfsr = *((volatile uint32_t*)0xE000ED28);  // CFSR
    (void)cfsr;  // Prevent optimization
    __asm("bkpt #0");
    for(;;);
}

void DebugMon_Handler(void)
{
    /* Debug Monitor */
    __asm("bkpt #0");
    for(;;);
}

// Default handler for unused interrupts
void Default_Handler(void)
{
    /* Unexpected interrupt */
    __asm("bkpt #0");
    for(;;);
}