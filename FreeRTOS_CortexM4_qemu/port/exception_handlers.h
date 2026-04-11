#ifndef EXCEPTION_HANDLERS_H
#define EXCEPTION_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

// Fault type definitions
#define FAULT_TYPE_UNKNOWN          0
#define FAULT_TYPE_ESCALATED        1
#define FAULT_TYPE_VECTOR_TABLE     2
#define FAULT_TYPE_MEMMANAGE        3
#define FAULT_TYPE_BUS_FAULT        4
#define FAULT_TYPE_USAGE_FAULT      5
#define FAULT_TYPE_STACK_OVERFLOW   6
#define FAULT_TYPE_HEAP_CORRUPTION  7

// Fault status register definitions for debugging
#define SCB_CFSR_IACCVIOL    (1UL << 0)   // Instruction access violation
#define SCB_CFSR_DACCVIOL    (1UL << 1)   // Data access violation
#define SCB_CFSR_MUNSTKERR   (1UL << 3)   // MemManage fault on unstacking
#define SCB_CFSR_MSTKERR     (1UL << 4)   // MemManage fault on stacking
#define SCB_CFSR_MLSPERR     (1UL << 5)   // MemManage fault during FP lazy state preservation
#define SCB_CFSR_MMARVALID   (1UL << 7)   // MemManage Fault Address Register valid

#define SCB_CFSR_IBUSERR     (1UL << 8)   // Instruction bus error
#define SCB_CFSR_PRECISERR   (1UL << 9)   // Precise data bus error
#define SCB_CFSR_IMPRECISERR (1UL << 10)  // Imprecise data bus error
#define SCB_CFSR_UNSTKERR    (1UL << 11)  // BusFault on unstacking
#define SCB_CFSR_STKERR      (1UL << 12)  // BusFault on stacking
#define SCB_CFSR_LSPERR      (1UL << 13)  // BusFault during FP lazy state preservation
#define SCB_CFSR_BFARVALID   (1UL << 15)  // BusFault Address Register valid

#define SCB_CFSR_UNDEFINSTR  (1UL << 16)  // Undefined instruction
#define SCB_CFSR_INVSTATE    (1UL << 17)  // Invalid state
#define SCB_CFSR_INVPC       (1UL << 18)  // Invalid PC
#define SCB_CFSR_NOCP        (1UL << 19)  // No coprocessor
#define SCB_CFSR_UNALIGNED   (1UL << 24)  // Unaligned access
#define SCB_CFSR_DIVBYZERO   (1UL << 25)  // Divide by zero


// Additional fault analysis registers
#define SCB_HFSR_VECTTBL     (1UL << 1)   // Vector table hard fault
#define SCB_HFSR_FORCED      (1UL << 30)  // Forced hard fault (escalated fault)
#define SCB_HFSR_DEBUGEVT    (1UL << 31)  // Debug event hard fault

// SHCSR register for exception status
#define SCB_SHCSR_MEMFAULTACT    (1UL << 0)   // MemManage fault active
#define SCB_SHCSR_BUSFAULTACT    (1UL << 1)   // Bus fault active
#define SCB_SHCSR_USGFAULTACT    (1UL << 3)   // Usage fault active
#define SCB_SHCSR_MEMFAULTENA    (1UL << 16)  // MemManage fault enable
#define SCB_SHCSR_BUSFAULTENA    (1UL << 17)  // Bus fault enable
#define SCB_SHCSR_USGFAULTENA    (1UL << 18)  // Usage fault enable

// Exception debug information structure
typedef struct {
    uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
} ExceptionFrame_t;

// Debug structure to capture hard fault information
typedef struct {
    uint32_t cfsr;           // Configurable Fault Status Register
    uint32_t hfsr;           // Hard Fault Status Register
    uint32_t mmfar;          // MemManage Fault Address Register
    uint32_t bfar;           // Bus Fault Address Register
    uint32_t shcsr;          // System Handler Control and State Register
    uint32_t stack_pointer;  // Stack pointer at fault
    uint32_t tick_count;     // FreeRTOS tick count at fault
    uint32_t heap_free;      // Free heap at fault
    uint32_t fault_type;     // Decoded fault type
    uint32_t fault_pc;       // Program counter at fault (from stack)
    uint32_t fault_lr;       // Link register at fault (from stack)
    uint32_t fault_r0;       // R0 register at fault (from stack)
} HardFaultDebugInfo_t;

/* Cortex-M3 Core Exception Handlers */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void Default_Handler(void);

/* FreeRTOS Exception Handlers (defined in FreeRTOS portable layer) */
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

#endif /* EXCEPTION_HANDLERS_H */