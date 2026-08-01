
#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"
#include <stdio.h>
#include <string.h>


extern GPIO_InitTypeDef BT_gpio_init_structure;
extern char received_char[2];

/*USART2-BLUETOOTH*/
 
#define BT_COM1_UART                     USART2
#define BT_COM1_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()
#define BT_COM1_CLK_DISABLE()            __HAL_RCC_USART2_CLK_DISABLE()

#define BT_COM1_TX_PIN                   GPIO_PIN_5
#define BT_COM1_TX_GPIO_PORT             GPIOD
#define BT_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define BT_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()
#define BT_COM1_TX_AF                    GPIO_AF7_USART2

#define BT_COM1_RX_PIN                   GPIO_PIN_6
#define BT_COM1_RX_GPIO_PORT             GPIOD
#define BT_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define BT_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()
#define BT_COM1_RX_AF                    GPIO_AF7_USART2
#define BT_COM_POLL_TIMEOUT             1000



#define Terminal_UART_MODULE                     USART3
#define Terminal_UART_CLK_ENABLE()             __HAL_RCC_USART3_CLK_ENABLE()
#define Terminal_UART_CLK_DISABLE()            __HAL_RCC_USART3_CLK_DISABLE()

#define Terminal_UART_TX_PIN                   GPIO_PIN_8
#define Terminal_UART_TX_GPIO_PORT             GPIOD
#define Terminal_UART_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define Terminal_UART_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()
#define Terminal_UART_TX_AF                    GPIO_AF7_USART3

#define Terminal_UART_RX_PIN                   GPIO_PIN_9
#define Terminal_UART_RX_GPIO_PORT             GPIOD
#define Terminal_UART_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define Terminal_UART_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()
#define Terminal_UART_RX_AF                    GPIO_AF7_USART3
#define Terminal_UART_POLL_TIMEOUT             1000



void Terminal_Console_Init(void);
void BlueTooth_Console_Init(void);
void BlueTooth_Display(char *text);
void Terminal_Display(char *text);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);