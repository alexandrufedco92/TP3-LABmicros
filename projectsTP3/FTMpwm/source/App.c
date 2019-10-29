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
//#include "comController2pc.h"
#include "uart.h"
#include "waveGen.h"
#include "timer.h"
#include <math.h>
#include "gpio.h"
#include "measureFreq.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MARK_KEY '1'
#define SPACE_KEY '0'

#define IS_MARK_FREQ(f) ( (f >= 1000) && (f <= 1400) )
#define IS_SPACE_FREQ(f) ( (f >= 2200) && (f <= 2600) )
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void FMcallback(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
enum{debugV1, debugV2};

int debugFlag = debugV2;

//DAC


//INPUT CAPTURE
int dif = 0;
int freq = 0;
//UART

char key = MARK_KEY;

/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{
	uart_cfg_t config;
	config.baudRate = 9600;
	config.nBits = 8;
	config.parity = NO_PARITY_UART;
	config.rxWaterMark = 5;
	config.txWaterMark = 2;
	config.mode = NON_BLOCKING_SIMPLE;

	uartInit (U0, config);

	if(debugFlag == debugV1)
	{
		WaveGenConfig_t waveConf;
		waveConf.freq = 50;
		waveConf.id = WAVE0_WAVEGEN;
		waveConf.mode = SAMPLES_WAVEGEN;
		waveConf.waveName = SIN_WAVEGEN;
		initWaveGen(&waveConf);
		SetTimer(MODULATION, 10, FMcallback);
	}
	else if(debugFlag == debugV2)
	{
		initFreqMeasure();

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
	if(debugFlag == debugV2)
	{
		//RX
		if(isNewMeasReady())
		{
			freq = getFreqMeasure();
		}
		if(IS_MARK_FREQ(freq))
		{
			key = MARK_KEY;
		}
		else if(IS_SPACE_FREQ(freq))
		{
			key = SPACE_KEY;
		}
		uartWriteMsg(U0, &key, 1);

		//TX  (por ahora se debuggea actualizando con interrupciones)

	}
	else if(debugFlag == debugV1)
	{
		//RX
		///ADC

		//TX (por ahora se debuggea actualizando con interrupciones)
	}


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


void FMcallback(void)
{
	static int i = 2;
	if((i % 2) == 0)
	{
		i = 3;
		updateWaveFreq(WAVE0_WAVEGEN, 1200);

	}
	else
	{
		i = 2;
		updateWaveFreq(WAVE0_WAVEGEN, 2200);
	}
}

/*******************************************************************************
 ******************************************************************************/
