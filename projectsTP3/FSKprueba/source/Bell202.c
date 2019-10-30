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

/***********************************************************
 * 				LOCAL FUNCTION DECLARATIONS
 ***********************************************************/
void ModulatorInit(void);

/************************************************************
 * 					STATIC VARIABLES
 ************************************************************/
modem_version_t selected_version;

/************************************************************
 * 				FUNCTIONS WITH GLOBAL SCOPE
 ************************************************************/

void ModemInit( modem_version_t version)
{
	selected_version = version;
	initResourcesController2pc(); //Initializes Communication with PC.
	ModulatorInit(version);
	DemodulatorInit(version);

	if(verion == DSP_VERSION)
	{

		//Set ADC configuration
		ADC_Config_t adc_config;
		adc_config.channel_sel = AD0;
		adc_config.clock_divide = DIVIDE_BY_1;
		adc_config.clock_type = BUS_CLOCK;
		adc_config.diffential_mode = false;
		adc_config.enable_cont_conversions = false;
		adc_config.enable_hardware_avg = false;
		adc_config.enable_interrupts = false;
		adc_config.id = FIRST_ADC;
		adc_config.low_power = true;
		adc_config.resolution = SIXTEEN_BITS;
		adc_config.trigger = HARDWARE_TRIGGER;
		adc_config.voltage_reference = DEFAULT;
		ADC_Init( &adc_config);
		//FSK Demodulator init
		DemodulatorInit();


	}




}

void ModemRun(void)
{

}

/******************************************************************
 * 					FUNCTION WITH LOCAL SCOPE
 ******************************************************************
