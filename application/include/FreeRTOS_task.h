#ifndef FREERTOS_TASK_H
#define FREERTOS_TASK_H

#define QUEUE_LENGTH 5
#define MSG_LEN 32

typedef struct
{
    char text[MSG_LEN];
} UartMessage_t;

extern void SM_Task_TerminalPrint(void *pvParameters);
extern void SM_Task_RedLedBlink(void *pvParameters);
extern void SM_Task_YellowLedBlink(void *pvParameters);

extern SemaphoreHandle_t xMutex_m7, xSemaphore_m7;
extern SemaphoreHandle_t xMutex_m4, xSemaphore_m4;
extern QueueHandle_t xUartQueue;

#endif