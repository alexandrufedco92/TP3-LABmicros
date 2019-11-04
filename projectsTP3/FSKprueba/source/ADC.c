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
#include "PIT.h"

/**********************************************************
 *					 DEFINES AND MACROS
 **********************************************************/
#define PIT1 5 //Alternate trigger for PIT 1
#define T_SAMPLE_PERIOD 83 //Sample time of ADC in microseconds.

/*********************************************************
 * 					STATIC VARIABLES
 *********************************************************/
bool initialized;
static ADC_Type * ADC;
static callback func;

/********************************************************
 * 				LOCAL FUNCTION DEFINITIONS
 ********************************************************/

bool SetChannelADC(ADC_Channel_t ch, bool diff, bool in_en);
void InitializeHardwareTrigger(void);

/*********************************************************
 * 					HEADER FUNCTIONS
 *********************************************************/
bool ADC_Init( const ADC_Config_t* config )
{
	bool valid = false;
	if(!initialized)
	{
		if( config->id == FIRST_ADC)
		{
			ADC = ADC0;
			SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;	//Clock gating para el ADC0
			NVIC_EnableIRQ(ADC0_IRQn);			//Enable ADC0 interrupts
			SIM->SOPT7 = (SIM->SOPT7 & ~SIM_SOPT7_ADC0ALTTRGEN_MASK)| SIM_SOPT7_ADC0ALTTRGEN(1); //Sets alternate trigger for ADC0
			SIM->SOPT7 = (SIM->SOPT7 & ~SIM_SOPT7_ADC0TRGSEL_MASK)| SIM_SOPT7_ADC0TRGSEL(PIT1);//Select PIT1 as alt trigger.
		}
		else
		{
			ADC = ADC1;
			SIM->SCGC3 |= SIM_SCGC3_ADC1_MASK;	//Clock gating for ADC1
			NVIC_EnableIRQ(ADC1_IRQn);			//Enable ADC1 interrupts
			SIM->SOPT7 = (SIM->SOPT7 & ~SIM_SOPT7_ADC1ALTTRGEN_MASK )| SIM_SOPT7_ADC1ALTTRGEN(1); //Sets alternate trigger for ADC1
			SIM->SOPT7 = (SIM->SOPT7 & ~SIM_SOPT7_ADC1TRGSEL_MASK )| SIM_SOPT7_ADC1TRGSEL(PIT1);//Select PIT1 as alt trigger.
		}
		func = config->intterupt_func;
		//Calibrate();						//Calibration

		//Update CFG1 register
		ADC->CFG1 = (ADC->CFG1 & (~ADC_CFG1_ADLPC_MASK)) | ( ADC_CFG1_ADLPC( config->low_power ) ); //Low power configuration.
		ADC->CFG1 = (ADC->CFG1 & (~ADC_CFG1_ADICLK_MASK)) | ADC_CFG1_ADICLK( config->clock_type ); //Clock select
		ADC->CFG1 = (ADC->CFG1 & (~ADC_CFG1_ADIV_MASK)) | ADC_CFG1_ADIV( config->clock_divide ); //Clock frequency divider
		ADC->CFG1 = (ADC->CFG1 & (~ADC_CFG1_ADLSMP_MASK)) | ADC_CFG1_ADLSMP( config->sample_time ); //Sample time
		ADC->CFG1 = (ADC->CFG1 & (~ADC_CFG1_MODE_MASK)) | ADC_CFG1_MODE( config->resolution ); //Number of bits of result.
		//Update SC2 register
		ADC->SC2 = (ADC->SC2 & (~ADC_SC2_ADTRG_MASK)) | ADC_SC2_ADTRG( config->trigger );				//Trigger select
		ADC->SC2 = (ADC->SC2 & (~ADC_SC2_REFSEL_MASK)) | ADC_SC2_REFSEL( config->voltage_reference );	//Reference select

		//Update SC3 register
		ADC->SC3 = (ADC->SC3 & (~ADC_SC3_ADCO_MASK)) | ADC_SC3_ADCO( config->enable_cont_conversions ); //Continuous conversion enable
		ADC->SC3 = (ADC->SC3 & (~ADC_SC3_AVGE_MASK)) | ADC_SC3_AVGE( config->enable_hardware_avg ); //Hardware average enable
		ADC->SC3 = (ADC->SC3 & (~ADC_SC3_AVGS_MASK)) | ADC_SC3_AVGS( config->samples_to_average ); //Average select.
		//Update SC1 registers
		valid = SetChannelADC(config->channel_sel, config->diffential_mode, config->enable_interrupts);
		//Initializes Hardware trigger
		if( config->trigger == HARDWARE_TRIGGER)
		{
			InitializeHardwareTrigger();
		}
		if( !valid )
		{
			return false;
		}

		initialized = true;
		return true;
	}
	else
	{
		return false;
	}

}

bool StartConversion(ADC_Channel_t channel, bool differential_mode, bool interrupt_enable)
{
	if( !(ADC->SC2 & ADC_SC2_ADTRG_MASK) )
	{
		SetChannelADC(channel, differential_mode, interrupt_enable);
		return true;
	}
	return false;
}

bool SetChannelADC(ADC_Channel_t channel, bool differential_mode, bool interrupt_enable)
{
	ADC->SC1[0] = ( (ADC->SC1[0]) & (~ADC_SC1_AIEN_MASK)) | ADC_SC1_AIEN( interrupt_enable );//Interrupt enable
	ADC->SC1[0] = ( (ADC->SC1[0]) & (~ADC_SC1_DIFF_MASK)) | ADC_SC1_DIFF( differential_mode ); //Differential mode
	if( (differential_mode) && ( (channel) > AD3) )
	{
		return false;	//Invalid configuration
	}
	else
	{
		ADC->SC1[0] = ( (ADC->SC1[0]) & (~ADC_SC1_ADCH_MASK)) | ADC_SC1_ADCH( channel ); //Select ADC input
		return true;
	}
}

void SetInterruptCallback(callback call)
{
	func = call;
}

bool IsConversionFinished(void)
{
	if( (ADC->SC1[0]) & ADC_SC1_COCO_MASK )
	{
		return true;
	}
	else
	{
		return false;
	}
}

ADC_Data_t GetConversionResult(void)
{
	return (ADC->R)[0];
}
/************************************************************
 * 					LOCAL FUNCTIONS
 ************************************************************/
void InitializeHardwareTrigger(void)
{
	//Pit initializaion for ADC sampling
	pit_config_t pit_config;
	pit_config.timerVal = T_SAMPLE_PERIOD;
	pit_config.timerNbr = 1;
	pit_config.chainMode = false;
	pit_config.pitCallback = NULL;
	PITinit();
	PITstartTimer(&pit_config);
}

void ADC0_IRQHandler(void)
{
	func();
}

void ADC1_IRQHandler(void)
{
	func();
}

