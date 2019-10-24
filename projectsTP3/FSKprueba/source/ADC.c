/*
 * ADC.c
 *
 *  Created on: Oct 23, 2019
 *      Author: G5
 */

/**********************************************************
 * 					INCLUDED HEADERS
 **********************************************************/
#include "ADC.h"
#include "MK64F12.h"

/*********************************************************
 * 					STATIC VARIABLES
 *********************************************************/
bool initialized;

/*********************************************************
 * 					HEADER FUNCTIONS
 *********************************************************/
void ADC_Init( const ADC_Config_t* config)
{
	ADC_Type * ADC;
	if(!initialized)
	{
		if( config->id == FIRST_ADC)
		{
			ADC = ADC0;
		}
		else
		{
			ADC = ADC1;
		}
		SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;	//Clock gating para el ADC0
		Calibrate();						//Calibration

		//Update CFG1 register
		ADC->CFG1 = ADC_CFG1_ADLPC( config->low_power ); //Low power configuration.
		ADC->CFG1 = ADC_CFG1_ADICLK( config->clock_type ); //Clock select
		ADC->CFG1 = ADC_CFG1_ADIV( config->clock_divide ); //Clock frequency divider
		ADC->CFG1 = ADC_CFG1_ADLSMP( config->sample_time ); //Sample time
		ADC->CFG1 = ADC_CFG1_MODE( config->resolution ); //Number of bits of result.
		//Update SC2 register
		ADC->SC2 = ADC_SC2_ADTRG( config->trigger );				//Trigger select
		ADC->SC2 = ADC_SC2_REFSEL( config->voltage_reference );	//Reference select

		//Update SC3 register
		ADC->SC3 = ADC_SC3_ADCO( config->enable_cont_conversions ); //Continuous conversion enable
		ADC->SC3 = ADC_SC3_AVGE( config->enable_hardware_avg ); //Hardware average enable
		ADC->SC3 = ADC_SC3_AVGS( config->samples_to_average ); //Average select.
		//Update SC1 registers
		ADC->SC1[0] = ADC_SC1_AIEN( config->enable_interrupts );//Interrupt enable
		ADC->SC1[0] = ADC_SC1_DIFF( config->diffential_mode ); //Differential mode
		if( (config->diffential_mode) && ( (config->channel_sel) > AD3) )
		{
			//Invalid configuration
		}
		else
		{
			ADC->SC1[0] =ADC_SC1_ADCH( config->channel_sel ); //Select ADC input
		}

		initialized = true;
	}

}

/************************************************************
 * 					LOCAL FUNCTIONS
 ************************************************************/
