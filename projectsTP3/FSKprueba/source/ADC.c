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

/**********************************************************
 *					 DEFINES AND MACROS
 **********************************************************/
#define PIT1 5 //Alternate trigger for PIT 1

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

void ADC0_IRQHandler(void)
{
	func();
}

void ADC1_IRQHandler(void)
{
	func();
}

/*
bool ADC_Calibrate (void)
{
	if(!initialized)
	{
		return false;
	}
	int32_t  Offset		= 0;
	uint32_t Minus	[7] = {0,0,0,0,0,0,0};
	uint32_t Plus	[7] = {0,0,0,0,0,0,0};
	uint8_t  i;
	uint32_t scr3;

	/// SETUP
	ADC->SC1[0] = 0x1F;
	scr3 = ADC->SC3;
	ADC->SC3 &= (ADC_SC3_AVGS(0x03) | ADC_SC3_AVGE_MASK);

	/// INITIAL CALIBRATION
	ADC->SC3 &= ~ADC_SC3_CAL_MASK;
	ADC->SC3 |=  ADC_SC3_CAL_MASK;
	while (!(ADC->SC1[0] & ADC_SC1_COCO_MASK));
	if (ADC->SC3 & ADC_SC3_CALF_MASK)
	{
		ADC->SC3 |= ADC_SC3_CALF_MASK;
		return false;
	}
	ADC->PG  = (0x8000 | ((adc->CLP0+adc->CLP1+adc->CLP2+adc->CLP3+adc->CLP4+adc->CLPS) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC->MG  = (0x8000 | ((adc->CLM0+adc->CLM1+adc->CLM2+adc->CLM3+adc->CLM4+adc->CLMS) >> (1 + TWO_POW_NUM_OF_CAL)));

	// FURTHER CALIBRATIONS
	for (i = 0; i < TWO_POW_NUM_OF_CAL; i++)
	{
		ADC->SC3 &= ~ADC_SC3_CAL_MASK;
		ADC->SC3 |=  ADC_SC3_CAL_MASK;
		while (!(ADC->SC1[0] & ADC_SC1_COCO_MASK));
		if (ADC->SC3 & ADC_SC3_CALF_MASK)
		{
			ADC->SC3 |= ADC_SC3_CALF_MASK;
			return 1;
		}
		Offset += (short)ADC->OFS;
		Plus[0] += (unsigned long)ADC->CLP0;
		Plus[1] += (unsigned long)ADC->CLP1;
		Plus[2] += (unsigned long)ADC->CLP2;
		Plus[3] += (unsigned long)ADC->CLP3;
		Plus[4] += (unsigned long)ADC->CLP4;
		Plus[5] += (unsigned long)ADC->CLPS;
		Plus[6] += (unsigned long)ADC->CLPD;
		Minus[0] += (unsigned long)ADC->CLM0;
		Minus[1] += (unsigned long)ADC->CLM1;
		Minus[2] += (unsigned long)ADC->CLM2;
		Minus[3] += (unsigned long)ADC->CLM3;
		Minus[4] += (unsigned long)ADC->CLM4;
		Minus[5] += (unsigned long)ADC->CLMS;
		Minus[6] += (unsigned long)ADC->CLMD;
	}
	ADC->OFS = (Offset >> TWO_POW_NUM_OF_CAL);
	ADC->PG  = (0x8000 | ((Plus[0] +Plus[1] +Plus[2] +Plus[3] +Plus[4] +Plus[5] ) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC->MG  = (0x8000 | ((Minus[0]+Minus[1]+Minus[2]+Minus[3]+Minus[4]+Minus[5]) >> (1 + TWO_POW_NUM_OF_CAL)));
	ADC->CLP0 = (Plus[0] >> TWO_POW_NUM_OF_CAL);
	ADC->CLP1 = (Plus[1] >> TWO_POW_NUM_OF_CAL);
	ADC->CLP2 = (Plus[2] >> TWO_POW_NUM_OF_CAL);
	ADC->CLP3 = (Plus[3] >> TWO_POW_NUM_OF_CAL);
	ADC->CLP4 = (Plus[4] >> TWO_POW_NUM_OF_CAL);
	ADC->CLPS = (Plus[5] >> TWO_POW_NUM_OF_CAL);
	ADC->CLPD = (Plus[6] >> TWO_POW_NUM_OF_CAL);
	ADC->CLM0 = (Minus[0] >> TWO_POW_NUM_OF_CAL);
	ADC->CLM1 = (Minus[1] >> TWO_POW_NUM_OF_CAL);
	ADC->CLM2 = (Minus[2] >> TWO_POW_NUM_OF_CAL);
	ADC->CLM3 = (Minus[3] >> TWO_POW_NUM_OF_CAL);
	ADC->CLM4 = (Minus[4] >> TWO_POW_NUM_OF_CAL);
	ADC->CLMS = (Minus[5] >> TWO_POW_NUM_OF_CAL);
	ADC->CLMD = (Minus[6] >> TWO_POW_NUM_OF_CAL);

	/// UN-SETUP
	ADC->SC3 = scr3;

	return true;
}
*/

