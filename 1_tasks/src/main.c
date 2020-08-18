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

void demoTaskOne(void *pvParameters)
{
	// task init code goes here
	printf("Starting task 1\n");
    int i = 0;

	while (1) {
        tumFUtilPrintTaskStateList();
		printf("Hello from task 1\n");

        if(i++ == 3)
            vTaskResume(handleTwo);

        vTaskDelay(1000);
	}
	// Should never get here!
}

void demoTaskTwo(void *pvParameters)
{
	printf("Starting task 2\n");

	while (1) {
        tumFUtilPrintTaskStateList();
		printf("Hello from task 2\n");
        vTaskDelay(2000);
	}
}

int main(int argc, char *argv[])
{
	xTaskCreate(demoTaskOne, "one", mainGENERIC_STACK_SIZE, NULL,
		    mainGENERIC_PRIORITY, &handleOne);
	xTaskCreate(demoTaskTwo, "two", mainGENERIC_STACK_SIZE, NULL,
		    mainGENERIC_PRIORITY, &handleTwo);

    tumFUtilPrintTaskStateList();
    vTaskSuspend(handleTwo);

    tumFUtilPrintTaskStateList();

	vTaskStartScheduler();

	//Should also never get here

	// Should also never get here
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
