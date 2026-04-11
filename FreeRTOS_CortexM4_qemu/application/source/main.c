#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "exception_handlers.h"


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
    vTaskStartScheduler();
}