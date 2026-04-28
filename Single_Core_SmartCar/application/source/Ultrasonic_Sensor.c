#include "Ultrasonic_Sensor.h"

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim31;
TIM_HandleTypeDef htim32;
TIM_HandleTypeDef htim4;

void MX_TIM3_Init(void)
{
  __HAL_RCC_TIM3_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct_TM3 = {0};

	  /*Configure GPIO pin : PB0(LED1) or PC8 */
	GPIO_InitStruct_TM3.Pin 		    	= GPIO_PIN_8;
	GPIO_InitStruct_TM3.Mode 		    	= GPIO_MODE_AF_PP;
	GPIO_InitStruct_TM3.Pull 		    		= GPIO_NOPULL;
	GPIO_InitStruct_TM3.Alternate 			= GPIO_AF2_TIM3;
	GPIO_InitStruct_TM3.Speed 		  		= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct_TM3);

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig       = {0};
  TIM_MasterConfigTypeDef sMasterConfig     = {0};
  TIM_OC_InitTypeDef sConfigOC              = {0};

  htim3.Instance 				        	= TIM3;
  htim3.Channel 				       		= HAL_TIM_ACTIVE_CHANNEL_3;//@ ADDED TIM2 AND CHANGED CHANNEL TO 1 FROM3
  htim3.Init.Prescaler 			    		= 64;
  htim3.Init.CounterMode 		    		= TIM_COUNTERMODE_UP;
  htim3.Init.Period 			      		= 100000;
  htim3.Init.ClockDivision 		  			= TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload 				= TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sSlaveConfig.SlaveMode 		    = TIM_SLAVEMODE_DISABLE;// @ FROM DISABLE 
  sSlaveConfig.InputTrigger 	  	= TIM_TS_ITR0;
  sSlaveConfig.TriggerPolarity 		= TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerPrescaler 	= TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter 	  	= 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode 	= TIM_MASTERSLAVEMODE_DISABLE;//@FROM DISABLE
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)//@ 0
  {
    Error_Handler();
  }

  sConfigOC.OCMode 			= TIM_OCMODE_PWM1;
  sConfigOC.Pulse 			= 10;
  sConfigOC.OCPolarity 		= TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity 	= 0;
  sConfigOC.OCIdleState 	= TIM_OCIDLESTATE_RESET;
  sConfigOC.OCFastMode 		= TIM_OCFAST_DISABLE;
 
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)//@OUTPUT CHANNEL SHOULD BE 1 OR 2
  {
    Error_Handler();
  }

  htim3.Instance->CR1 |= TIM_CR1_OPM;
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);//@CHANNEL CHANGED TO 1
}





void MX_TIM4_Init(void)
{
    __HAL_RCC_TIM4_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct_TM4_1 = {0};
    
    // Configure GPIO pin for Timer 4 Channel 1 (Input Capture)
    GPIO_InitStruct_TM4_1.Pin = GPIO_PIN_6;
    GPIO_InitStruct_TM4_1.Mode = GPIO_MODE_AF_PP;  // Fixed: was MODE_AF
    GPIO_InitStruct_TM4_1.Pull = GPIO_NOPULL;
    GPIO_InitStruct_TM4_1.Alternate = GPIO_AF2_TIM4;
    GPIO_InitStruct_TM4_1.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_TM4_1);

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    // Timer 4 base configuration - make it longer than Timer 3 period
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 64;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 65535;  // Increased to avoid overflow issues
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    // Remove slave configuration that was causing resets
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    // Configure Input Capture Channel 1 for Rising Edge
    TIM_IC_InitTypeDef sConfigIC1 = {0};
    sConfigIC1.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC1.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC1.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC1.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC1, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }

    // Configure Input Capture Channel 2 for Falling Edge
    TIM_IC_InitTypeDef sConfigIC2 = {0};
    sConfigIC2.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC2.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    sConfigIC2.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC2.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC2, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_TIM_IC_Init(&htim4) != HAL_OK) {
        Error_Handler();
    }

    // Enable interrupts
    HAL_NVIC_SetPriority(TIM4_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);

    // Start input capture on both channels
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
}






uint32_t first_capture = 0;
uint32_t second_capture = 0;
uint32_t timer3_capture = 0;
uint8_t capture_done = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            // Rising edge captured
            first_capture = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
            capture_done = 0;  // Reset flag
        } 
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            // Falling edge captured
            second_capture = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_2);
            capture_done = 1;  // Set flag to indicate both captures are done
        }

        // Only process when both captures are available
        if (capture_done) {
            char first[50];
            snprintf(first, sizeof(first), "Rising edge: %lu\r\n", first_capture);
            Terminal_Display(first);
            
            char second[50];
            snprintf(second, sizeof(second), "Falling edge: %lu\r\n", second_capture);
            Terminal_Display(second);

            char tim3cap[50];
            timer3_capture = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_3);
            snprintf(tim3cap, sizeof(tim3cap), "timer3_capture: %lu\r\n", timer3_capture);
            Terminal_Display(tim3cap);

            // Calculate pulse width
            uint32_t pulse_width;
            if (second_capture > first_capture) {
                pulse_width = second_capture - first_capture;
            } else {
                // Handle timer overflow case
                pulse_width = (65535 - first_capture) + second_capture;
            }
            
            char pulse_msg[50];
            snprintf(pulse_msg, sizeof(pulse_msg), "Pulse width: %lu\r\n", pulse_width);
            Terminal_Display(pulse_msg);
            
            capture_done = 0;  // Reset for next measurement
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1); // Toggle LED on GPIOE pin 1
        }
    }
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
}