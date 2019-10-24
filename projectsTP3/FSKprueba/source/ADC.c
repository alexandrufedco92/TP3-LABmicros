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
 * 					HEADER FUNCTIONS
 *********************************************************/
void ADC_Init( const ADC_Config_t* config)
{
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;	//Clock gating para el ADC0
	Calibrate();						//Calibration

	//Update CFG1 register
	ADC0->CFG1 = ADC_CFG1_ADLPC( config->low_power ); //Low power configuration.
	ADC0->CFG1 = ADC_CFG1_ADICLK( config->clock_type ); //Clock select
	ADC0->CFG1 = ADC_CFG1_ADIV( config->clock_divide ); //Clock frequency divider
	ADC0->CFG1 = ADC_CFG1_ADLSMP( config->sample_time ); //Sample time
	ADC0->CFG1 = ADC_CFG1_MODE( config->resolution ); //Number of bits of result.
	//Update SC2 register
	ADC0->SC2 = ADC_SC2_ADTRG( config->trigger );				//Trigger select
	ADC0->SC2 = ADC_SC2_REFSEL( config->voltage_reference );	//Reference select

	//Update SC3 register
	ADC0->SC3 = ADC_SC3_ADCO( config->enable_cont_conversions ); //Continuous conversion enable
	ADC0->SC3 = ADC_SC3_AVGE( config->enable_hardware_avg ); //Hardware average enable
	ADC0->SC3 = ADC_SC3_AVGS( config->samples_to_average ); //Average select.
	//Update SC1 registers
	ADC0->SC1[0] = ADC_SC1_AIEN( config->enable_interrupts );
	ADC0->SC1[0] = ADC_SC1_DIFF( config->diffential_mode );
	ADC0->SC1[0] =
}

/************************************************************
 * 					LOCAL FUNCTIONS
 ************************************************************/
