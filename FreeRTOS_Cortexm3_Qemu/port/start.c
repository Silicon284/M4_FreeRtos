#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "exception_handlers.h"

// Exception debug information structure
typedef struct {
    uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
} ExceptionFrame_t;

// Global variable to hold exception information for debugging
volatile ExceptionFrame_t g_exception_frame;

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

// Fault type definitions
#define FAULT_TYPE_UNKNOWN          0
#define FAULT_TYPE_ESCALATED        1
#define FAULT_TYPE_VECTOR_TABLE     2
#define FAULT_TYPE_MEMMANAGE        3
#define FAULT_TYPE_BUS_FAULT        4
#define FAULT_TYPE_USAGE_FAULT      5
#define FAULT_TYPE_STACK_OVERFLOW   6
#define FAULT_TYPE_HEAP_CORRUPTION  7


// Global variable to store hard fault debug information
volatile HardFaultDebugInfo_t g_hardFaultInfo = {0};

// Stack overflow hook function
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* This function will be called if a task overflows its stack, if
     * configCHECK_FOR_STACK_OVERFLOW is set to 1 or 2. */
    (void) xTask;
    (void) pcTaskName;

    // Use FreeRTOS interrupt disable macro instead
    //portDISABLE_INTERRUPTS();

    
    /* Loop forever if we get here */
    for(;;);
}

// Malloc failed hook function
void vApplicationMallocFailedHook(void)
{
    /* This function will be called if malloc fails */
    __asm("bkpt #0");
    for(;;);
}

// Cortex-M3 Exception Handlers
void NMI_Handler(void)
{
    /* Non-Maskable Interrupt */
    __asm("bkpt #0");
    for(;;);
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
    if (g_hardFaultInfo.stack_pointer >= 0x20000000 && 
        g_hardFaultInfo.stack_pointer < 0x20002000) {
        
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
    if (g_hardFaultInfo.stack_pointer < 0x20000100) {
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

void vApplicationIdleHook(void)
{
    /* This function is called on each cycle of the idle task. In this case it
     * does nothing useful, but it could be used to:
     * - Put the processor into a low power mode
     * - Perform background processing
     * - Feed a watchdog timer
     * - Toggle an LED to show system activity
     */
    
    /* Simple implementation - just increment a counter */
    static volatile uint32_t ulIdleCounter = 0;
    ulIdleCounter = 40;
    
    /* Optional: Put processor into sleep mode to save power
     * Uncomment the following line if you want to enable sleep mode:
     */
    // __WFI();  // Wait For Interrupt - puts CPU to sleep until next interrupt
    
    /* Optional: Toggle an LED or GPIO to show idle activity
     * This can be useful for debugging to see when the system is idle
     */
    
    /* Note: Keep this function lightweight and fast.
     * Don't call any blocking FreeRTOS API functions from here.
     * Don't use printf or other heavy operations.
     */
}

SemaphoreHandle_t xMutex, xSemaphore;

volatile enum {
    RED = 5,
    YELLOW = 6,
    GREEN = 4
} traffic_signal;

static volatile uint32_t runTimeCounter = 0;
volatile int a = 0;
volatile int CommonData[4];

void configureTimerForRunTimeStats(void){
    runTimeCounter = 0;
    /* Configure and start timer or rely on existing periodic ISR. */
}

uint32_t getRunTimeCounterValue(void){
    return xTaskGetTickCount();
}

static void v1Task(void *pvParameters){
    while (1) {
        if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            traffic_signal = RED;
            a++;
            for(uint8_t i=0;i<4;i++){
                CommonData[i] = a+i;
            }
            xSemaphoreGive(xMutex);
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void v2Task(void *pvParameters) {
    while (1) {
        if(xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) { 
            if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
                traffic_signal = YELLOW;
                volatile int consumed_a = a;  // Read the produced data
                volatile int consumed_data[4];
                for(uint8_t i=0; i<4; i++){
                    consumed_data[i] = CommonData[i];
                }
                xSemaphoreGive(xMutex);                           
            }            
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void v3Task(void *pvParameters) {

    while (1) {
        if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            traffic_signal = GREEN;
            //a++;
            xSemaphoreGive(xMutex);            
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void main(void) {

    BaseType_t xReturn;
    xMutex = xSemaphoreCreateMutex();
    xSemaphore = xSemaphoreCreateBinary();

    // Verify semaphore creation was successful
    if(xMutex == NULL || xSemaphore == NULL) {
        // Handle error - semaphore creation failed
        while(1);
    }
    vQueueAddToRegistry( (QueueHandle_t) xMutex, "Signal Mutex" );
    vQueueAddToRegistry( (QueueHandle_t) xSemaphore, "Signal Semaphore Flag" );

    xReturn = xTaskCreate(v1Task, "T1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xReturn = xTaskCreate(v2Task, "T2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xReturn = xTaskCreate(v3Task, "T3", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();
}

void start(void) {
    
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;

    while (dst < &_edata) {
        *dst++ = *src++;
    }

    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    main();
    return;
}
