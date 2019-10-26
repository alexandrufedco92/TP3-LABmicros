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
#include "waveGen.h"
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

void FMcallback(void);
void FTMtimerCallback(FTMchannels ch);
void FTMicCallback(FTMchannels ch);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
enum{debugPWM, debugInputCapture, debugDAC};

int debugFlag = debugPWM;

//DAC


//INPUT CAPTURE
int dif = 0;
/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{
	if(debugFlag == debugDAC)
	{
		WaveGenConfig_t waveConf;
		waveConf.freq = 50;
		waveConf.id = WAVE0_WAVEGEN;
		waveConf.mode = SAMPLES_WAVEGEN;
		waveConf.waveName = SIN_WAVEGEN;
		initWaveGen(&waveConf);
		SetTimer(MODULATION, 40, FMcallback);
	}
	else if(debugFlag == debugInputCapture)
	{
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
		FTMconfigIC.nModule = FTM2_INDEX;
		FTMconfigIC.nChannel = FTM_CH0;
		FTMconfigIC.edge = UP_EDGE;
		FTMconfigIC.nTicks = 0xFFFF;
		FTMconfigIC.numOverflows = 0;
		FTMconfigIC.prescaler = FTM_PSCX32;
		FTMconfigIC.p2callback = FTMicCallback;

		FTMinit(&FTMtimerConfig);
		//disableFTMinterrupts(FTM0_INDEX);
		FTMinit(&FTMconfigIC);
		//enableFTMinterrupts(FTM0_INDEX);

		gpioMode (PORTNUM2PIN(PB, 9), OUTPUT);
	}
	else if(debugFlag == debugPWM)
	{
		WaveGenConfig_t waveConf;
		waveConf.freq = 50;
		waveConf.id = WAVE0_WAVEGEN;
		waveConf.mode = PWM_WAVEGEN;
		waveConf.waveName = SIN_WAVEGEN;
		initWaveGen(&waveConf);
	}






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


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void FTMtimerCallback(FTMchannels ch)
{

	if(ch == FTM_CH0)
	{
		updateCnV(FTM0_INDEX, FTM_CH0, getCnV(FTM0_INDEX, FTM_CH0)+ 100);
		if(getCnV(FTM0_INDEX, FTM_CH0) >= getMOD_FTM(FTM0_INDEX, FTM_CH0))
		{
			updateCnV(FTM0_INDEX, FTM_CH0, getCNTIN_FTM(FTM0_INDEX, FTM_CH0));
		}
		gpioToggle(PORTNUM2PIN(PB, 9));
	}
	else //overflow
	{
		//gpioToggle(PORTNUM2PIN(PB, 9));
	}
}

void FTMicCallback(FTMchannels ch)
{
	static int i = 0;
	static int firstMeasure = 0;
	static int secondMeasure = 0;
	 if(ch == FTM_CH0)
	{
		i++;
		if(i == 1)
		{
			firstMeasure = getCnV(FTM2_INDEX, FTM_CH0);
		}
		else if(i == 2)
		{
			secondMeasure = getCnV(FTM2_INDEX, FTM_CH0);
			dif = secondMeasure - firstMeasure;
		}

	}
	else //overflow
	{
		//gpioToggle(PORTNUM2PIN(PB, 9));
	}
}



void FMcallback(void)
{
	static int i = 2;
	if((i % 2) == 0)
	{
		i = 3;
		updateWaveFreq(WAVE0_WAVEGEN, 50);
	}
	else
	{
		i = 2;
		updateWaveFreq(WAVE0_WAVEGEN, 10);
	}
}

/*******************************************************************************
 ******************************************************************************/
