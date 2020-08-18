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

SemaphoreHandle_t mySyncSignal = NULL;
struct my_struct {
	SemaphoreHandle_t lock;
	int my_variable;
};

struct my_struct my_struct_instance = { .lock = NULL };

void demoTaskOne(void *pvParameters)
{
	while (1) {
		if (mySyncSignal) {
			if (xSemaphoreTake(mySyncSignal, portMAX_DELAY) ==
			    pdTRUE) {
				printf("Hello from task 1\n");
			} else
				printf("Couldn't take semaphore\n");
		}
	}
	// Should never get here!
}

void demoTaskTwo(void *pvParameters)
{
	while (1) {
		xSemaphoreGive(mySyncSignal);
		printf("Hello from task 2\n");
		if (xSemaphoreTake(my_struct_instance.lock, portMAX_DELAY) ==
		    pdTRUE) {
			// We have the lock and we can modify the structure's constents
			my_struct_instance.my_variable++;
			xSemaphoreGive(my_struct_instance.lock);
		}
		vTaskDelay(1);
	}
}

void demoTaskThree(void *pvParameters)
{
	while (1) {
		printf("Hello from task 3\n");
		if (xSemaphoreTake(my_struct_instance.lock, 0) == pdTRUE) {
			printf("Lock obtained in task 3\n");
            xSemaphoreGive(my_struct_instance.lock);
		} else {
            tumFUtilPrintTaskStateList();
		}
		vTaskDelay(2);
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

	mySyncSignal = xSemaphoreCreateBinary();
	if (mySyncSignal == NULL)
		return -1;

	my_struct_instance.lock = xSemaphoreCreateMutex();
	if (my_struct_instance.lock == NULL)
		return -1;

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
