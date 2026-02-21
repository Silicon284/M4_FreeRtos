#ifndef EXCEPTION_HANDLERS_H
#define EXCEPTION_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* EXCEPTION_HANDLERS_H */