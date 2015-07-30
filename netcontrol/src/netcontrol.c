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

// TODO: insert other include files here
#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"

// TODO: insert other definitions and declarations here

int main(void)
{
	/* Inicializando o sistema. */
	SystemInit();

	/* Inicializa o Clock */
	SystemCoreClockUpdate();

	/* create task to blink led */
	xTaskCreate(blinktask, "blink", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* should never reach here! */
	while(1);
}
