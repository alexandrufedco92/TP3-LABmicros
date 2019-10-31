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
	initResourcesController2pc(); 	//Initializes Communication with PC.
	ModulatorInit();				//Initializes FSK modulator
	DemodulatorInit();				//Initializes FSK demodulator
}

void ModemRun(void)
{
	float sample = 0;
	bool digital_symbol = true;
	char recieved[FRAME_SIZE];

	//FSK demodulation
	if( NeedDemodulation() ) //Checks for sample
	{
		sample = GetConversionResult(); //ACA directamente pongo  el shape
		digital_symbol = DemodulateSignal( sample ); //Demodulates sample
		if( IsDemodulationFinished() )
		{
			sendMessage2pc( GetFrameMsg(), FRAME_SIZE); //Send Frame to next station.
		}
	}
	//FSK modulation
	if( isMsg() )
	{
		recieveMessageFromPC(recieved, FRAME_SIZE);
		ModulateFSK( ); //Chequear que parametro mandar aca
	}
}

/******************************************************************
 * 					FUNCTION WITH LOCAL SCOPE
 ******************************************************************
