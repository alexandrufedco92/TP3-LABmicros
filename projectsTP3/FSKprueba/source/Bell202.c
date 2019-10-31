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
#include "comController2pc.h"
#include "FSK_Modulator.h"
#include "FSK_Demodulator.h"
#include "ADC.h"
/*************************************************************
 * 					DEFINES AND MACROS
 *************************************************************/
#define FRAME_SIZE 11

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
	config.baudRate = 9600;
	config.nBits = 8;
	config.parity = NO_PARITY_UART;
	config.rxWaterMark = 5;
	config.txWaterMark = 2;
	config.mode = NON_BLOCKING_SIMPLE;

	uartInit (U0, config);
	ModulatorInit();				//Initializes FSK modulator
	DemodulatorInit();				//Initializes FSK demodulator
}

void ModemRun(void)
{
	bool digital_symbol = true;
	char recieved[FRAME_SIZE];

	//FSK demodulation
	if( NeedDemodulation() ) //Checks for sample
	{
		digital_symbol = DemodulateSignal(); //Starts Demodulation
		if( IsDemodulationFinished() )
		{
			sendMessage2pc( GetFrameMsg(), FRAME_SIZE); //Send Frame to next station.
		}
	}
	//FSK modulation
	if( isMsg() )
	{
		recieveMessageFromPC(recieved, FRAME_SIZE);
		ModulateFSK( recieved ); //Chequear que parametro mandar aca
	}
}

/******************************************************************
 * 					FUNCTION WITH LOCAL SCOPE
 ******************************************************************
