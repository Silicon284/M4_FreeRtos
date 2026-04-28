#include "FreeRTOS.h"
#include "task.h"

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