#include "Bluetooth_Console.h"


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void BT_Error_Handler(void);

COM_InitTypeDef BspCOMInit;
UART_HandleTypeDef BT_UART;
UART_HandleTypeDef Terminal_UART;
UART_InitTypeDef BT_COMInit;
GPIO_InitTypeDef BT_gpio_init_structure;
GPIO_InitTypeDef Terminal_gpio_init_structure;
char received_char[2];


void BlueTooth_Console_Init(void){

  /* Enable GPIO clock */
  BT_COM1_TX_GPIO_CLK_ENABLE();
  BT_COM1_RX_GPIO_CLK_ENABLE();

  /* Enable USART clock */
  BT_COM1_CLK_ENABLE();

  /*ENABLE INTERRUPT*/
  HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);  // Changed priority to 5 (lower priority than system)
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  /* Configure USART Tx as alternate function */
  BT_gpio_init_structure.Pin = BT_COM1_TX_PIN;
  BT_gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  BT_gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  BT_gpio_init_structure.Pull = GPIO_NOPULL;
  BT_gpio_init_structure.Alternate = BT_COM1_TX_AF;
  HAL_GPIO_Init(BT_COM1_TX_GPIO_PORT, &BT_gpio_init_structure);

  /* Configure USART Rx as alternate function */
  BT_gpio_init_structure.Pin = BT_COM1_RX_PIN;
  BT_gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  BT_gpio_init_structure.Pull = GPIO_PULLUP;
  BT_gpio_init_structure.Alternate = BT_COM1_RX_AF;
  HAL_GPIO_Init(BT_COM1_RX_GPIO_PORT, &BT_gpio_init_structure);
   /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */

  BT_UART.Instance        = USART2;
  BT_UART.Init.BaudRate   = 115200;
  BT_UART.Init.WordLength = UART_WORDLENGTH_8B;
  BT_UART.Init.StopBits   = UART_STOPBITS_1;
  BT_UART.Init.Parity     = UART_PARITY_NONE;
  BT_UART.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  BT_UART.Init.Mode       = UART_MODE_TX_RX;
  BT_UART.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&BT_UART) != HAL_OK)
  {
    BT_Error_Handler();
  }

  // Verify UART is ready
  if (BT_UART.gState != HAL_UART_STATE_READY)
  {
    BT_Error_Handler();
  }

    // Debug: Check UART registers
  char reg_debug[100];
  sprintf(reg_debug, "USART2 CR1=0x%08lX, CR2=0x%08lX, RQR=0x%08lX\r\n", USART2->CR1, USART2->CR2, USART2->RQR);
  Terminal_Display(reg_debug);

  /* Infinite loop */
  Terminal_Display("Hello from STM32 On-Board USART MODULE!\r\n");
  
  // Check if UART receive interrupt setup is successful
  HAL_StatusTypeDef status = HAL_UART_Receive_IT(&BT_UART, (uint8_t*)received_char, 2);
  if (status != HAL_OK)
  {
    char error_msg[] = "UART Receive IT Failed!\r\n";
    Terminal_Display(error_msg);
    BT_Error_Handler();
  }
  else
  {
    char success_msg[] = "UART Receive IT Started Successfully!\r\n";
    Terminal_Display(success_msg);
  }
}

void BlueTooth_Display(char* text) {
    // Check if text is not NULL
    if (text != NULL) {
        HAL_UART_Transmit(&BT_UART, (uint8_t*)text, strlen(text), 1000);
    }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2) // Check if the interrupt is from USART2
  {
    // Toggle LED to show interrupt is working
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Green LED

    char debug_msg[] = "ISR: Received '";
    Terminal_Display(debug_msg);
    Terminal_Display(received_char);
    char end_msg[] = "'\r\n";
    Terminal_Display(end_msg);

    // Continue receiving in interrupt mode
    HAL_UART_Receive_IT(&BT_UART, (uint8_t*)received_char, 2);
  }
}

// Add error callback for debugging
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
    char error_msg[] = "UART Error Occurred during ISR!\r\n";
    Terminal_Display(error_msg);    // Debug: Check UART registers


    // Clear error and restart reception
    __HAL_UART_CLEAR_OREFLAG(&BT_UART);
    __HAL_UART_CLEAR_NEFLAG(&BT_UART);
    __HAL_UART_CLEAR_FEFLAG(&BT_UART);
    __HAL_UART_CLEAR_PEFLAG(&BT_UART);
    

    // Restart reception
    HAL_UART_Receive_IT(&BT_UART, (uint8_t*)received_char, 2);
  }
}

void BT_Error_Handler(void)
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

void Terminal_Console_Init(void){
      /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */

     /* Enable GPIO clock */
  Terminal_UART_TX_GPIO_CLK_ENABLE();
  Terminal_UART_RX_GPIO_CLK_ENABLE();

  /* Enable USART clock */
  Terminal_UART_CLK_ENABLE();


  /* Configure USART Tx as alternate function */
  Terminal_gpio_init_structure.Pin = Terminal_UART_TX_PIN;
  Terminal_gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  Terminal_gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  Terminal_gpio_init_structure.Pull = GPIO_NOPULL;
  Terminal_gpio_init_structure.Alternate = Terminal_UART_TX_AF;
  HAL_GPIO_Init(Terminal_UART_TX_GPIO_PORT, &Terminal_gpio_init_structure);

  /* Configure USART Rx as alternate function */
  Terminal_gpio_init_structure.Pin = Terminal_UART_RX_PIN;
  Terminal_gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  Terminal_gpio_init_structure.Pull = GPIO_PULLUP;
  Terminal_gpio_init_structure.Alternate = Terminal_UART_RX_AF;
  HAL_GPIO_Init(Terminal_UART_RX_GPIO_PORT, &Terminal_gpio_init_structure);
   /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */

  Terminal_UART.Instance       = USART3;
  Terminal_UART.Init.BaudRate   = 115200;
  Terminal_UART.Init.WordLength = UART_WORDLENGTH_8B;
  Terminal_UART.Init.StopBits   = UART_STOPBITS_1;
  Terminal_UART.Init.Parity     = UART_PARITY_NONE;
  Terminal_UART.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  Terminal_UART.Init.Mode         = UART_MODE_TX_RX;
  Terminal_UART.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&Terminal_UART) != HAL_OK)
  {

  }

  if (Terminal_UART.gState != HAL_UART_STATE_READY)
  {

  }  

    char text[] = "Hello from STM32 On-Board USART MODULE!\r\n";
    Terminal_Display(text);
}

void Terminal_Display(char* text) {
    if(*text != '\0'){
      HAL_UART_Transmit(&Terminal_UART, (uint8_t*)text, strlen(text), 1000);
    }
}