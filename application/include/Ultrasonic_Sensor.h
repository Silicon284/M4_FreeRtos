#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"
#include "stm32h7xx_hal_tim.h"
#include "Bluetooth_Console.h"

void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
extern void SM_Ultrasonic_Trigger_Pulse(void);
extern void SM_Ultrasonic_Calculate_distance(void);