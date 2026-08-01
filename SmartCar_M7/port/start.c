#include <stdint.h>
#include "main.h"
#include "stm32h7xx_hal.h"

extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;
extern uint32_t _sbss;
extern uint32_t _ebss;

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

    /* Enable HSEM EXTI event line for D2 (M4) domain wake-up from STOP.
     * EXTI line 78 = HSEM event for CPU2/M4. This matches the reference
     * system_stm32h7xx_dualcore_boot_cm4_cm7.c SystemInit (CORE_CM7 path).
     * Must be set before M4 executes WFI in its STOP sequence. */
    //EXTI_D2->EMR3 |= 0x4000UL;

    main();
    return;
}
