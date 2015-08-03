/*
===============================================================================
 Name        : netcontrol.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

#include "lpc17xx_gpio.h"

// TODO: insert other include files here
#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include "dumb.h"

// TODO: insert other definitions and declarations here
void InitHardware(void)
{
	uint8_t i;

	/* Setting Digital Output. */
	for(i = OUTPUTPORTMINIOPIN; i <= OUTPUTPORTMAXIOPIN; i++)
	{
		GPIO_SetDir(OUTPUTPORTIO, (1 << i), 1);
		GPIO_ClearValue(OUTPUTPORTIO, (1 << i));
	}
}

int main(void)
{
	/* Initialize System. */
	SystemInit();

	/* Inicialize Clock */
	SystemCoreClockUpdate();

	/* Inicialize Hardware. */
	InitHardware();

	/* Create task to ethernet */
	xTaskCreate(ethernettask, "ethernet", (configMINIMAL_STACK_SIZE * 2), NULL, (tskIDLE_PRIORITY + 1), NULL);

	/* Create task to blink led */
	xTaskCreate(blinktask, "blink", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* should never reach here! */
	while(1);
}
