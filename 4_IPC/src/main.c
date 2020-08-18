#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "TUM_FreeRTOS_Utils.h"

#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)

TaskHandle_t handleOne = NULL;
TaskHandle_t handleTwo = NULL;
TaskHandle_t handleThree = NULL;

QueueHandle_t myQueue = NULL;

void demoTaskOne(void *pvParameters)
{
    int my_tmp_int;

	while (1) {
        for (int i = 0; i < 3; i++)
        {
            my_tmp_int = xTaskGetTickCount();
            xQueueSend(myQueue, &my_tmp_int, portMAX_DELAY);
        }

        if ( xTaskNotify(handleThree, xTaskGetTickCount(), eSetBits) == pdTRUE)
            printf("Task notification sent!\n");

		vTaskDelay(1);
	}
	// Should never get here!
}

void demoTaskTwo(void *pvParameters)
{
    int my_tmp_int;

	while (1) {
        if( xQueueReceive(myQueue, &my_tmp_int, portMAX_DELAY) == pdTRUE)
            printf("Recv: %d\n", my_tmp_int);
        else 
            printf("Recv failed\n");
	}
}

void demoTaskThree(void *pvParameters)
{
    int task_notification_val;
	while (1) {
        if( (task_notification_val = ulTaskNotifyTake(pdFALSE, portMAX_DELAY)))
            printf("Task notification recv: %d\n", task_notification_val);
	}
}

int main(int argc, char *argv[])
{
	xTaskCreate(demoTaskOne, "one", mainGENERIC_STACK_SIZE, NULL, 1,
		    &handleOne);
	xTaskCreate(demoTaskTwo, "two", mainGENERIC_STACK_SIZE, NULL, 1,
		    &handleTwo);
	xTaskCreate(demoTaskThree, "three", mainGENERIC_STACK_SIZE, NULL, 1,
		    &handleThree);

    myQueue = xQueueCreate(10, sizeof(int));

	vTaskStartScheduler();

	//Should also never get here

	return EXIT_SUCCESS;
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vMainQueueSendPassed(void)
{
	/* This is just an example implementation of the "queue send" trace hook. */
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vApplicationIdleHook(void)
{
#ifdef __GCC_POSIX__
	struct timespec xTimeToSleep, xTimeSlept;
	/* Makes the process more agreeable when using the Posix simulator. */
	xTimeToSleep.tv_sec = 1;
	xTimeToSleep.tv_nsec = 0;
	nanosleep(&xTimeToSleep, &xTimeSlept);
#endif
}
