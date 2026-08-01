

void* memcpy (void *dst, const void *src, unsigned int size) {
    unsigned char *d = (unsigned char*) dst;
    const unsigned char *s = (const unsigned char*) src;

    while (size > 0){
        *d++ = *s++;
        size--;
    }

    return dst;
}

// Add strcpy function
char* strcpy(char *dst, const char *src) {
    char *original_dst = dst;
    
    while (*src != '\0') {
        *dst++ = *src++;
    }
    *dst = '\0';  // null terminate
    
    return original_dst;
}

// Add strlen function
unsigned int strlen(const char *str) {
    unsigned int len = 0;
    
    while (*str != '\0') {
        len++;
        str++;
    }
    
    return len;
}

/* Combined SysTick handler: always increments HAL tick,
   only calls FreeRTOS handler after the scheduler has started. */
#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler(void);
extern BaseType_t xTaskGetSchedulerState(void);


void HSEM2_IRQHandler(void) {
    HAL_HSEM_IRQHandler();  // reads MISR, clears ICR, calls FreeCallback (weak no-op)
}

void SysTick_Handler(void) {
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}

// Add basic snprintf function
int snprintf(char *str, unsigned int size, const char *format, ...) {
    // Simple implementation - just copy format string for now
    // In a real implementation, you'd handle format specifiers
    unsigned int i = 0;
    const char *src = format;
    
    if (size == 0) return 0;
    
    while (*src != '\0' && i < (size - 1)) {
        str[i++] = *src++;
    }
    str[i] = '\0';
    
    return i;
}