
#include "math.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"
#include <stdio.h>
#include "Bluetooth_Console.h"

void Display_IMU_Euler_Angles(void);
void calculate_angles_complementary(int16_t ax, int16_t ay, int16_t az, 
                                   int16_t gx, int16_t gy, int16_t gz,
                                   int16_t *pitch, int16_t *roll, int16_t *yaw);

I2C_HandleTypeDef hi2c1;

static int16_t ax, ay, az, gx, gy, gz, temp;
int16_t pitch, roll, yaw;

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler_IMU(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {    
    Terminal_Display("Hello from IMU Error Handler\r\n");
    //HAL_Delay(10000); // Delay to avoid flooding the UART
  }
  /* USER CODE END Error_Handler_Debug */
}

void MX_I2C4_Init(void)
{
  __HAL_RCC_I2C1_CLK_ENABLE();
    
    /* USER CODE BEGIN I2C4_Init 0 */
	  /* USER CODE BEGIN I2C4_Init 0 */
	GPIO_InitTypeDef GPIO_InitStruct_I2C_SCL = {0};

  /* USER CODE BEGIN I2C1  */
  /*Configure GPIO pin : PB8(SCL)*/
	GPIO_InitStruct_I2C_SCL.Pin 		= GPIO_PIN_8;
	GPIO_InitStruct_I2C_SCL.Mode 		= GPIO_MODE_AF_OD;
	GPIO_InitStruct_I2C_SCL.Pull 		= GPIO_NOPULL;
	GPIO_InitStruct_I2C_SCL.Alternate 	= GPIO_AF4_I2C1;
	GPIO_InitStruct_I2C_SCL.Speed 		= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_I2C_SCL);
  /* USER CODE END TIM3_Init 0 */

  /* USER CODE BEGIN I2C4_Init 0 */
	GPIO_InitTypeDef GPIO_InitStruct_I2C_SDA = {0};

		  /* USER CODE BEGIN I2C1  */
		  /*Configure GPIO pin : PB9(SDA)*/
		GPIO_InitStruct_I2C_SDA.Pin 		= GPIO_PIN_9;
		GPIO_InitStruct_I2C_SDA.Mode 		= GPIO_MODE_AF_OD;
		GPIO_InitStruct_I2C_SDA.Pull 		= GPIO_NOPULL;
		GPIO_InitStruct_I2C_SDA.Alternate 	= GPIO_AF4_I2C1;
		GPIO_InitStruct_I2C_SDA.Speed 		= GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_I2C_SDA);
  /* USER CODE END I2C4 0 */
  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00909FCE;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler_IMU();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler_IMU();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler_IMU();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
  /* USER CODE BEGIN I2C4_Init 0 */
	GPIO_InitTypeDef GPIO_InitStruct_IMU_GPIO = {0};
  /* USER CODE END I2C1_Init 2 */
  		  /* USER CODE BEGIN I2C1  */
		  /*Configure GPIO pin : PB0(SDA)*/
		GPIO_InitStruct_IMU_GPIO.Pin 		= GPIO_PIN_0;
		GPIO_InitStruct_IMU_GPIO.Mode 	= GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct_IMU_GPIO.Pull 	= GPIO_NOPULL;
		GPIO_InitStruct_IMU_GPIO.Speed 	= GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_IMU_GPIO);

    uint8_t IMUAdd = 0x68<<1;
    uint16_t MemAddSize = 1;
    uint8_t pData = 0x01;
    
    uint16_t Size = 1;
    uint32_t Timeout = 100;

    HAL_I2C_Mem_Write(&hi2c1, IMUAdd, 0x38, MemAddSize, &pData,  Size,  Timeout);
    pData = 250;
    HAL_I2C_Mem_Write(&hi2c1, IMUAdd, 0x19, MemAddSize, &pData,  Size,  Timeout); 
    pData = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, IMUAdd, 0x1A, MemAddSize, &pData,  Size,  Timeout);     
  /*ENABLE INTERRUPT*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);  // Changed priority to 5 (lower priority than system)
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

void IMU_Raw_Read(void) {

    uint8_t IMUAdd = 0x68<<1;
    uint16_t MemAddSize = 1;
    uint8_t pData = 0x24;
    
    uint16_t Size = 1;
    uint32_t Timeout = 100;

    //HAL_I2C_Mem_Write(&hi2c1, IMUAdd, 0x6B, MemAddSize, &pData,  Size,  Timeout);
    
    uint8_t IMUData[14];
    uint16_t IMU_bytes = 14;

    HAL_I2C_Mem_Read(&hi2c1, IMUAdd, 0x3B, MemAddSize, &IMUData[0],  IMU_bytes,  Timeout);
    ax = IMUData[0] << 8 | IMUData[1];
    ay = IMUData[2] << 8 | IMUData[3];
    az = IMUData[4] << 8 | IMUData[5];

    temp = IMUData[6] << 8 | IMUData[7];

    gx = IMUData[8] << 8 | IMUData[9];
    gy = IMUData[10] << 8 | IMUData[11];
    gz = IMUData[12] << 8 | IMUData[13];

    pData = 0;
    //HAL_I2C_Mem_Write(&hi2c1, IMUAdd, 0x6B, MemAddSize, &pData,  Size,  Timeout);

    calculate_angles_complementary(ax,ay, az, gx, gy, gz, &pitch, &roll, &yaw);
    Display_IMU_Euler_Angles();
}


void Display_IMU_Euler_Angles(void) {
    UartMessage_t msg;
    snprintf(msg.text, MSG_LEN, "%d : %d : %d \n\r", ax, ay, az);
    if (xQueueSend(xUartQueue, &msg, pdMS_TO_TICKS(100)) != pdPASS)
    {
        /* Queue full, message not sent */
    }
  UartMessage_t msg; 
}

void calculate_angles_complementary(int16_t ax, int16_t ay, int16_t az, 
                                   int16_t gx, int16_t gy, int16_t gz,
                                   int16_t *pitch, int16_t *roll, int16_t *yaw)
{
  float_t SAMPLE_TIME = 0.25;
  float_t ALPHA = 0.96;
  static float pitch_filtered = 0.0f;
  static float roll_filtered = 0.0f;
  static float yaw_integrated = 0.0f;
  char IMU_msgs[100];

  
  // Calculate angles from accelerometer (in degrees)
  float_t pitch_accel = atan2f(-ax, sqrtf(ay*ay + az*az)) * 180.0f / M_PI;
  float_t roll_accel = atan2f(ay, sqrtf(ax*ax + az*az)) * 180.0f / M_PI;
  
  // Integrate gyroscope data (in degrees)
  float_t pitch_gyro = pitch_filtered + gy * SAMPLE_TIME;
  float_t roll_gyro = roll_filtered + gx * SAMPLE_TIME;
  
  sprintf (IMU_msgs, "%d : %d : %d \n\r", (int16_t)pitch_accel, (int16_t)gz, (int16_t)yaw_integrated);
  Terminal_Display(IMU_msgs);
  // Apply complementary filter
  pitch_filtered = ALPHA * pitch_gyro + (1.0f - ALPHA) * pitch_accel;
  roll_filtered = ALPHA * roll_gyro + (1.0f - ALPHA) * roll_accel;
  
  // Yaw integration (gyroscope only, as accelerometer can't measure yaw)
  yaw_integrated += (gz-106) * SAMPLE_TIME;
  
  // Return filtered values
  *pitch = (int16_t)pitch_filtered;
  *roll = (int16_t)roll_filtered;
  *yaw = (int16_t)yaw_integrated;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {  
  IMU_Raw_Read();
}