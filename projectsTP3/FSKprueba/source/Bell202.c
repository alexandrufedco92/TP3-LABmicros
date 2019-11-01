/*
 * Bell202.c
 *
 *  Created on: Oct 30, 2019
 *      Author: G5
 */

/************************************************************
 *  				HEADERS INCLUDED
 ************************************************************/
#include "Bell202.h"
#include "uart.h"
#include "bitStreamQueue.h"
#include "FSK_Modulator.h"
#include "FSK_Demodulator.h"
#include "ADC.h"
/*************************************************************
 * 					DEFINES AND MACROS
 *************************************************************/
#define FRAME_SIZE 11
#define MAX_RECIEVED_SIZE 20
#define DATA_SIZE 8

/***********************************************************
 * 				LOCAL FUNCTION DECLARATIONS
 ***********************************************************/
void ModulatorInit(void);

/************************************************************
 * 					STATIC VARIABLES
 ************************************************************/

/************************************************************
 * 				FUNCTIONS WITH GLOBAL SCOPE
 ************************************************************/

void ModemInit( void)
{

	uart_cfg_t config;
	config.baudRate = 1200;
	config.nBits = 8;
	config.parity = EVEN_PARITY_UART;
	config.rxWaterMark = 5;
	config.txWaterMark = 2;
	config.mode = NON_BLOCKING_SIMPLE;

	uartInit (U0, config);			//Initializes UART module

	bitStreamQueueInit();			//Initializes data queues
	DemodulatorInit();				//Initializes FSK demodulator
	ModulatorInit();				//Initializes FSK modulator
}

void ModemRun(void)
{
	char recieved[MAX_RECIEVED_SIZE];
	uint8_t cant_recieved = 0;	//Recieved bytes from UART.
	char msg2send = 0;

	//FSK demodulation
	if( NeedDemodulation() ) //Checks for sample
	{
		DemodulateSignal(); //Starts Demodulation
		if( IsFrameReady() )
		{

			msg2send = GetFrame();
			uartWriteMsg(U0, &msg2send , 1); //Sends data frame

		}
	}

	//FSK modulation

	cant_recieved = uartIsRxMsg(U0);
	if( cant_recieved )
	{
		uartReadMsg(U0, recieved, cant_recieved );
		pushString(recieved, cant_recieved);
	}

}

/******************************************************************
 * 					FUNCTION WITH LOCAL SCOPE
 ******************************************************************/
