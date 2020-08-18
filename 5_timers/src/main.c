#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "TUM_FreeRTOS_Utils.h"

#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)

TaskHandle_t handleOne = NULL;
TimerHandle_t myTimer = NULL;

void myTimerCallback(TimerHandle_t xTimer)
{
	printf("Hello from the timer\n");
}

void demoTaskOne(void *pvParameters)
{
	int i = 1;
	TickType_t base_period = 1000;
	while (1) {
		printf("Task is woke\n");
		xTimerChangePeriod(myTimer, pdMS_TO_TICKS(base_period / i++), portMAX_DELAY);
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
	// Should never get here!
}

int main(int argc, char *argv[])
{
	xTaskCreate(demoTaskOne, "one", mainGENERIC_STACK_SIZE, NULL, 1,
		    &handleOne);

	myTimer = xTimerCreate("My timer", pdMS_TO_TICKS(1000), pdFALSE,
			       (void *)0, myTimerCallback);

	if (xTimerStart(myTimer, 0) == pdFAIL) {
		printf("Timer start failed\n");
		return -1;
	}

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
