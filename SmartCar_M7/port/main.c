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
#include "stm32h7xx_hal_rcc_ex.h"
#include "stm32h7xx_hal_rcc.h"
#include "FreeRTOS_task.h"

#define HSEM_ID_0 (0U)

static volatile uint32_t runTimeCounter = 0;

char Terminal_text[] = "Hello from Core M7 !\r\n";

void BoardStartUp (void);
void Error_Handler(void);

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  // RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  // RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  // RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  // RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  // if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  // {
  //   Error_Handler();
  // }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
  RCC_OscInitStruct.PLL.PLLFRACN = 3072;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

      // ... (HAL_RCC_ClockConfig for SYSCLK, AHB, APBx, etc.) ...

    /** Initializes the Peripherals clocks
    */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC; // Add other used peripherals
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_CLKP; // Or RCC_ADCCLKSOURCE_PLL2P, or RCC_ADCCLKSOURCE_PERIPHCLK


    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

void configureTimerForRunTimeStats(void){
    runTimeCounter = 0;
    /* Configure and start timer or rely on existing periodic ISR. */
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
    HAL_Delay(10000); // Delay to avoid flooding the UART
  }
  /* USER CODE END Error_Handler_Debug */
}

void BoardStartUp (void) {

    int32_t timeout;

    /* Wait until CPU2 boots and enters in stop mode or timeout*/
    timeout = 0x0FFFFFFF;
    while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) /*&& (timeout-- > 0)*/);
    if ( timeout < 0 ) {
    Error_Handler();
    }

    /*HW semaphore Clock enable*/
    __HAL_RCC_HSEM_CLK_ENABLE();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
    HSEM notification */    
    HAL_HSEM_FastTake(HSEM_ID_0); /*Take HSEM */

    // Enable BusFault so it routes to BusFault_Handler (not HardFault)
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;

  __HAL_RCC_GPIOB_CLK_ENABLE();

  //BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_YELLOW);  
  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  // STM32H7 specific: Ensure USART2 is in the correct power domain
  __HAL_RCC_D2SRAM1_CLK_ENABLE();
  __HAL_RCC_D2SRAM2_CLK_ENABLE();
  __HAL_RCC_D2SRAM3_CLK_ENABLE();

  Terminal_Console_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();

  MX_I2C4_Init();
  BlueTooth_Console_Init();

  //MotorControl_Init();
  //MotorControl_ADC_Init();
  
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
  // ③ Release HSEM #0 — this is the signal that wakes M4

  HAL_HSEM_Release(HSEM_ID_0, 0);
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET)/* && (timeout-- > 0)*/);
  if ( timeout < 0 ) {
    Error_Handler();
  }
}

void main(void) {

    BoardStartUp();

    BaseType_t xReturn;

    xMutex_m7 = xSemaphoreCreateMutex();
    xSemaphore_m7 = xSemaphoreCreateBinary();
    xUartQueue = xQueueCreate(QUEUE_LENGTH, sizeof(UartMessage_t));
    xSemaphoreGive(xSemaphore_m7);   // make the first take succeed

    // Verify semaphore creation was successful
    if(xMutex_m7 == NULL || xSemaphore_m7 == NULL || xUartQueue == NULL) { /* Handle error - semaphore creation failed*/
        while(1);
    }
    vQueueAddToRegistry( (QueueHandle_t) xMutex_m7, "Signal Mutex" );
    vQueueAddToRegistry( (QueueHandle_t) xSemaphore_m7, "Signal Semaphore Flag" );

    xReturn = xTaskCreate(SM_Task_RedLedBlink, "RedLedBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xReturn = xTaskCreate(SM_Task_TerminalPrint, "TerminalPrint", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xReturn = xTaskCreate(SM_Task_Motor_CurrentA, "MotorCurrentA", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xReturn = xTaskCreate(SM_Task_Ultrasonic_Trigger_Pulse, "Ultrasonic_Trigger", configMINIMAL_STACK_SIZE, NULL, 1, Ultrasonic_Trigger_Handle);
    xReturn = xTaskCreate(SM_Task_Ultrasonic_Calculate_distance, "Ultrasonic_distance", configMINIMAL_STACK_SIZE, NULL, 1, Ultrasonic_distance_Handle);  

    vTaskStartScheduler();
}