#ifndef _CONFIG_H__
#define _CONFIG_H__

extern void configureTimerForRunTimeStats(void);
extern uint32_t getRunTimeCounterValue(void);

#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (5<<4)
#define configMINIMAL_STACK_SIZE                (0x7F)
#define configIDLE_TASK_STACK_SIZE              (0x100)
#define configMAX_PRIORITIES                    (6)
#define configUSE_PREEMPTION                    1
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configTOTAL_HEAP_SIZE                   (0x1000)
#define configTICK_RATE_HZ                      100
#define configCPU_CLOCK_HZ                      8000000

/* 2. Idle Task Name Customization */
#define configIDLE_TASK_NAME                    "IDLE"  // Default name override
#define configNUMBER_OF_CORES                   1    // Single core system
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

#define configUSE_MUTEXES                       1
#define configUSE_COUNTING_SEMAPHORES           0
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_TIME_SLICING                  1
#define configUSE_NEWLIB_REENTRANT              0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0
#define INCLUDE_vTaskDelay                      1
#define configUSE_TRACE_FACILITY                1    /* Enables task enumeration APIs */
#define configUSE_STATS_FORMATTING_FUNCTIONS    0    /* Enables vTaskList(), vTaskGetRunTimeStats() */
#define INCLUDE_vTaskSuspend                    0

#define configRECORD_STACK_HIGH_ADDRESS         0
#define configQUEUE_REGISTRY_SIZE               10
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_MALLOC_FAILED_HOOK            0
#define configCHECK_FOR_STACK_OVERFLOW          0
#define portGET_RUN_TIME_COUNTER_VALUE          getRunTimeCounterValue
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS  configureTimerForRunTimeStats

/* Add assertion for debugging */
#define configASSERT(x) if(!(x)) { __asm("bkpt #0"); for(;;); }

#endif /* _CONFIG_H__ */