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

    /* SEVONPEND: WFE wakes on any pending NVIC interrupt (required for HSEM DSTOP wake-up) */
    //SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

    main();
    return;
}
