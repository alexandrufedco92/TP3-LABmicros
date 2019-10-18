/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
/*

*/
#include "comController2pc.h"
#include "DAC.h"
#include "timer.h"
#include <math.h>
#include "gpio.h"
#include "FTM.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void senoidalCallback(void);
void FTMtimerCallback(FTMchannels ch);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
int dif = 0;
/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{
	DACconfig_t DACconfig;
	initResourcesController2pc();
	InitializeTimers();
	DACinit(DAC0_ID, &DACconfig);
	writeDACvalue(DAC0_ID, 2.0);
	//SetTimer(SENOIDAL, 1, senoidalCallback);
	/*typedef struct{
		FTMmodules nModule; //FTM0_INDEX or FTM1_INDEX, etc

		FTMchannels nChannel; //FTM_CH0, FTM_CH1, etc.

		FTMmodes mode; //FTM_TIMER, FTM_INPUT_CAPTURE, etc

		FTMprescaler prescaler; //x1, x2, x4, etc

		FTM_TIMERcountModes countMode;

		FTMedgeDetect edge;

		FTM_callback_t p2callback;   //pointer to subroutine called when FTM interrupts

		int numOverflows; //quantity of overflows before interrupt

		int nTicks; //count limit of FTM counter

		int CnV;

	}FTMconfig_t;*/
	FTMconfig_t FTMtimerConfig;
	FTMtimerConfig.mode = FTM_OUTPUT_COMPARE;
	FTMtimerConfig.nModule = FTM0_INDEX;
	FTMtimerConfig.nChannel = FTM_CH0;
	FTMtimerConfig.countMode = UP_COUNTER;
	FTMtimerConfig.prescaler = FTM_PSCX32;
	FTMtimerConfig.CnV = 0;
	FTMtimerConfig.nTicks = 0xFFFF;
	FTMtimerConfig.numOverflows = 0;
	FTMtimerConfig.p2callback = FTMtimerCallback;

	FTMconfig_t FTMconfigIC;
	FTMconfigIC.mode = FTM_INPUT_CAPTURE;
	FTMconfigIC.nModule = FTM0_INDEX;
	FTMconfigIC.nChannel = FTM_CH2;
	FTMconfigIC.edge = UP_EDGE;
	FTMconfigIC.nTicks = 0xFFFF;
	FTMconfigIC.numOverflows = 0;
	FTMconfigIC.prescaler = FTM_PSCX128;
	FTMconfigIC.p2callback = FTMtimerCallback;

	FTMinit(&FTMtimerConfig);
	disableFTMinterrupts(FTM0_INDEX);
	FTMinit(&FTMconfigIC);
	enableFTMinterrupts(FTM0_INDEX);

	gpioMode (PORTNUM2PIN(PB, 9), OUTPUT);


}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	/* Main program to generate a sinusoidal signal with de DAC, controlling the frequency
	 * by an input voltage to the ADC:
	 * The program waits a timer's interrupt that increments a global float variable t and
	 * put to the DAC the nearest value of sin(2*PI*t/T), while the variable T is being
	 * changed by the input of the ADC. It's important to decrement the variable t doing t - T
	 * only when t is higher than T.
	 */

	/*Main program to probe FTM (TIMER, OUTPUT COMPARE, INPUT CAPTURE and PWM):
	 *
	 */
}

void senoidalCallback(void)
{
	static float t = 0.0;

	if(t >= 0.010)
	{
		t = 0.000;
	}
	else
	{
		t += 0.001;
	}
	float valueAux = 1.0000 + sin(2*M_PI*t*100); //100Hz
	writeDACvalue(DAC0_ID, valueAux);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void FTMtimerCallback(FTMchannels ch)
{
	static int i = 0;
	static int firstMeasure = 0;
	static int secondMeasure = 0;
	if(ch == FTM_CH0)
	{
		updateCnV(FTM0_INDEX, FTM_CH0, getCnV(FTM0_INDEX, FTM_CH0)+ 100);
		if(getCnV(FTM0_INDEX, FTM_CH0) >= getMOD_FTM(FTM0_INDEX, FTM_CH0))
		{
			updateCnV(FTM0_INDEX, FTM_CH0, getCNTIN_FTM(FTM0_INDEX, FTM_CH0));
		}
		gpioToggle(PORTNUM2PIN(PB, 9));
	}
	else if(ch == FTM_CH2)
	{
		i++;
		if(i == 1)
		{
			firstMeasure = getCnV(FTM0_INDEX, FTM_CH2);
		}
		else if(i == 2)
		{
			secondMeasure = getCnV(FTM0_INDEX, FTM_CH2);
			dif = secondMeasure - firstMeasure;
		}

	}
	else //overflow
	{
		//gpioToggle(PORTNUM2PIN(PB, 9));
	}
}



/*******************************************************************************
 ******************************************************************************/
