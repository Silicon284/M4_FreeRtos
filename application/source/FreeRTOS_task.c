#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
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

SemaphoreHandle_t xMutex_m7, xSemaphore_m7;
SemaphoreHandle_t xMutex_m4, xSemaphore_m4;
QueueHandle_t xUartQueue;
TaskHandle_t Ultrasonic_Trigger_Handle = NULL, Ultrasonic_distance_Handle = NULL;


volatile enum {
    RED = 5,
    YELLOW = 6,
    GREEN = 4
} traffic_signal;

void SM_Task_TerminalPrint(void *pvParameters){      
    UartMessage_t rxMsg;
    while(1)
    {
        if (xQueueReceive(xUartQueue, &rxMsg, portMAX_DELAY) == pdPASS)
        {
            Terminal_Display(rxMsg.text);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void SM_Task_RedLedBlink(void *pvParameters) {
    UartMessage_t msg;
    while (1) {
        if(xSemaphoreTake(xSemaphore_m7, portMAX_DELAY) == pdTRUE) { 
            if(xSemaphoreTake(xMutex_m7, portMAX_DELAY) == pdTRUE) {
                traffic_signal = YELLOW;
                xSemaphoreGive(xMutex_m7);                
            }
            xSemaphoreGive(xSemaphore_m7);          
        }
        snprintf(msg.text, MSG_LEN, "Data from core m7\r\n");
        if (xQueueSend(xUartQueue, &msg, pdMS_TO_TICKS(100)) != pdPASS)
        {
            /* Queue full, message not sent */
        }
        BSP_LED_Toggle(LED_RED);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void SM_Task_YellowLedBlink(void *pvParameters) {
    UartMessage_t msg;
    while (1) {
        if(xSemaphoreTake(xMutex_m4, portMAX_DELAY) == pdTRUE) {
            traffic_signal = GREEN;
            xSemaphoreGive(xMutex_m4);            
        }
        snprintf(msg.text, MSG_LEN, "Data from core m4\r\n");
        if (xQueueSend(xUartQueue, &msg, pdMS_TO_TICKS(100)) != pdPASS) {
            /* Queue full, message not sent */
        }
        BSP_LED_Toggle(LED_YELLOW);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void SM_Task_Motor_CurrentA (void *pvParameters){
    while(1)
    {
        MotorControl_ReadCurrentA();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void SM_Task_Ultrasonic_Trigger_Pulse(void *pvParameters) {
  SM_Ultrasonic_Trigger_Pulse();
}

void SM_Task_Ultrasonic_Calculate_distance(void *pvParameters) {
  SM_Ultrasonic_Calculate_distance();
  vTaskResume(Ultrasonic_Trigger_Handle);
}