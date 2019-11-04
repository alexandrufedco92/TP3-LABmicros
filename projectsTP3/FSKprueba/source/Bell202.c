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
	//DemodulatorInit();				//Initializes FSK demodulator
	//ModulatorInit();				//Initializes FSK modulator
}
typedef enum{NOT_SENDING, SENDING}tansmission_state_t;

void ModemRun(void)
{
	char recieved;
	char aux =0;
	char msg2send = 0;
	tansmission_state_t state = NOT_SENDING;

	//Prueba solo con UART

	if( ! isQueueEmpty() )
		{
			if( state == NOT_SENDING)
			{
				msg2send = popChar();
				uartWriteMsg(U0, &msg2send , 1); //Sends data frame
				state = SENDING;
			}
			else
			{
				if( uartIsTxMsgComplete(U0))
				{
					state = NOT_SENDING;
				}
			}
		}


	if( uartIsRxMsg(U0) )
		{
			uartReadMsg(U0, &recieved, 1 );
			pushChar(recieved);
		}


/*
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

	if( uartIsRxMsg(U0) )
	{
		uartReadMsg(U0, &recieved, 1 );
		aux = AdjustParity(recieved);
		pushChar(aux);
	}
*/

}

/******************************************************************
 * 					FUNCTION WITH LOCAL SCOPE
 ******************************************************************/
