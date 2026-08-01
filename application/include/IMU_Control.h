#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"
#include <stdio.h>
#include "Bluetooth_Console.h"
#include "stm32h7xx_hal_i2c.h"
#include "stm32h7xx_hal_gpio.h"

void MX_I2C4_Init(void);
void IMU_Raw_Acc_Read(void);
extern void HAL_GPIO_EXTI_Callback(uint16_t);
extern I2C_HandleTypeDef hi2c1;