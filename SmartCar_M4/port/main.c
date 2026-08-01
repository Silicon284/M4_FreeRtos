#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "exception_handlers.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"
#include "Bluetooth_Console.h"
#include "IMU_Control.h"
#include "MotorControl.h"
#include "Ultrasonic_Sensor.h"
#include "FreeRTOS_task.h"

#define HSEM_ID_0 (0U)

static volatile uint32_t runTimeCounter = 0;


void BoardStartUp (void);

void configureTimerForRunTimeStats(void){
    runTimeCounter = 0;    /* Configure and start timer or rely on existing periodic ISR. */
}

uint32_t getRunTimeCounterValue(void){
    return xTaskGetTickCount();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {    
    Terminal_Display("Hello from Error Handler\r\n");
    //HAL_Delay(10000); // Delay to avoid flooding the UART
  }
  /* USER CODE END Error_Handler_Debug */
}

void BoardStartUp (void) {
    HAL_Init();
    __HAL_RCC_D2SRAM2_CLK_ENABLE();
    Terminal_Console_Init();
    //HAL_InitTick(TICK_INT_PRIORITY);
}

void main(void) {
    BSP_LED_Init(LED_RED);
    BSP_LED_Toggle(LED_RED);

    __HAL_RCC_HSEM_CLK_ENABLE();     /*HW semaphore Clock enable*/
    
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0)); /* Activate HSEM notification for Cortex-M4*/
    /*
    Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
    perform system initialization (system clock config, external memory configuration.. )
    */
    HAL_PWREx_ClearPendingEvent();
    HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
     /* Clear HSEM flag */
    __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

    BoardStartUp();

    BaseType_t xReturn;
    xMutex_m4 = xSemaphoreCreateMutex();
    xSemaphore_m4 = xSemaphoreCreateBinary();
    xUartQueue = xQueueCreate(QUEUE_LENGTH, sizeof(UartMessage_t));

    xSemaphoreGive(xSemaphore_m4);   // make the first take succeed

    // Verify semaphore creation was successful
    if(xMutex_m4 == NULL || xSemaphore_m4 == NULL ||xUartQueue == NULL) {
        // Handle error - semaphore creation failed
        while(1);
    }

    vQueueAddToRegistry( (QueueHandle_t) xMutex_m4, "Signal Mutex" );
    vQueueAddToRegistry( (QueueHandle_t) xSemaphore_m4, "Signal Semaphore Flag" );

    xReturn = xTaskCreate(SM_Task_YellowLedBlink, "YellowLedBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xReturn = xTaskCreate(SM_Task_TerminalPrint, "TerminalPrint", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();
}