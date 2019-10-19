/*
 * main.c
 *
 *  Created on: May 1, 2015
 *      Author: Juan Pablo VEGA - Laboratorio de Microprocesadores
 */

#include "hardware.h"
#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"
#include "UART.h"
#include "ADC.h"
#include "SysTick.h"


#define __FOREVER__ 	for(;;)



int main (void)
{
 	 	 	 	hw_Init();
 	 	 	 	PORT_Init();
 	 	 		GPIO_Init();
 	 	 		UART_Init();
 	 	 		SysTick_Init();

 	 	 		ADC_Init();



 	 	 			UART_Send_Data('A');
 	 	 			UART_Send_Data('B');
 	 	 			UART_Send_Data('C');



 	 	// 		hw_DisableInterrupts();

 	 	 		__FOREVER__;

			// Enable interrupts
			//hw_EnableInterrupts();

}

