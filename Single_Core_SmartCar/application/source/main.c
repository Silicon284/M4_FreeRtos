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
    HAL_Delay(10000); // Delay to avoid flooding the UART
  }
  /* USER CODE END Error_Handler_Debug */
}

void BoardStartUp (void) {
    /* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();
  /* Activate HSEM notification for Cortex-M4*/
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(0));
  /*
  Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
  perform system initialization (system clock config, external memory configuration.. )
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(0));

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  HAL_MspInit();
  /* USER CODE BEGIN Init */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_YELLOW);
  BSP_LED_Init(LED_RED);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  // STM32H7 specific: Ensure USART2 is in the correct power domain
  __HAL_RCC_D2SRAM1_CLK_ENABLE();
  __HAL_RCC_D2SRAM2_CLK_ENABLE();
  __HAL_RCC_D2SRAM3_CLK_ENABLE();
  
  // Wait for USART2 clock to stabilize
  HAL_Delay(1);
  Terminal_Console_Init();
  MX_TIM3_Init();
  //HAL_Delay(1);
  MX_TIM4_Init();

  //MX_I2C4_Init();
  //BlueTooth_Console_Init();

  // Initialize Motor Control System
 // MotorControl_Init();
  //MotorControl_ADC_Init();
  
  char motor_init_msg[] = "Motor Control and ADC Initialized!\r\n";
  Terminal_Display(motor_init_msg);

 HAL_StatusTypeDef statusI2C = HAL_OK;

  char timer_init_msg[100];
  uint8_t IMUAdd = 0x68<<1;
  uint16_t MemAddSize = 1;
  uint8_t pData = 0x04;
  uint8_t tData[2];
  uint16_t Size = 1;
  uint32_t Timeout = 100;
  int16_t temperature_raw = 0;
  float_t temperature = 0.0;

  //HAL_I2C_Mem_Write(&hi2c1, IMUAdd, 0x6B, MemAddSize, &pData,  Size,  Timeout);
}

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

    BoardStartUp();
    vTaskStartScheduler();
}