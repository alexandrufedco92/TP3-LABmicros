/*
 * FSK_Modulator.c
 *
 *  Created on: Oct 30, 2019
 *      Author: G5
 */

/************************************************************
 * 					INCLUDED HEADERS
 ************************************************************/
#include "FSK_Modulator.h"
#include "waveGen.h"
#include "measureFreq.h"
#include "bitStreamQueue.h"
#include "PIT.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MARK_FREQ	1200U /* Frequency in Hz. */
#define SPACE_FREQ	2200U /* Frequency in Hz. */
#define IDLE_FREQ	MARK_FREQ
#define MOD_PERIOD_US	833 /* Period of the modulated signal. */
#define IDLE_VAL	true /* Logic Value for Idle state. */
#define LOGIC_1_VAL	true
#define LOGIC_0_VAL	false
#define DATA_SIZE 8

static unsigned int index = 0;
static bool idle = true;
static bool currVal;
static bool nextVal;
static WAVEGENid wId;

/*******************************************************************************
 * LOCAL FUNCTION DECLARATIONS
 ******************************************************************************/

/**
 * @brief Modulates signal. Updates Frequency of signal according to data in bitStreamQueue.
 */
void ModulateFSK(void);

/*************************************************************
 * FUNCTIONS WITH GLOBAL SCOPE
 *************************************************************/
void ModulatorInit(void)
{
	//Initialize flags
	idle = true;
	index = 0;
	/* Initialize Wave Generator. */
	WaveGenConfig_t waveConf;
	waveConf.freq = IDLE_FREQ;
	waveConf.id = WAVE0_WAVEGEN;
	wId = WAVE0_WAVEGEN;
	waveConf.waveName = SIN_WAVEGEN;
	#ifdef DAC_VERSION
		waveConf.mode = SAMPLES_WAVEGEN;
	#else	//PWM Version
		initFreqMeasure();
		waveConf.mode = PWM_WAVEGEN;
	#endif
	initWaveGen(&waveConf);

	/* Initialize Modulator Timer. */

	PITinit();
	pit_config_t configP3 = { 	MOD_PERIOD_US, 	/* Value of timer in us. */
								2, 				/* Number of PIT timer. */
								false,  		/* True if timer in Chain Mode. */
								ModulateFSK 	/* Callback for interrupt. NULL if interrupt is disabled. */
							};
	PITstartTimer(&configP3);

	/* Initialize local variables. */
	currVal = IDLE_VAL; /* Idle starting value is logic 1. */
	nextVal = true;
}

/*******************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

void ModulateFSK(void){
	if(isQueueEmpty()){ /* Data Queue empty. */
		if(currVal != IDLE_VAL){ /* Transition to Idle state. */
			updateWaveFreq(wId, IDLE_FREQ);
			currVal = IDLE_VAL;
			idle = true;
		}
	}
	else{ /* Data Queue has new value. */
		if( idle ){
			idle = false;
			updateWaveFreq(wId, SPACE_FREQ); //Send start bit.
		}
		if( index == DATA_SIZE){
			updateWaveFreq(wId, MARK_FREQ); //Send parity bit.
			index++;
		}
		else  if( index == DATA_SIZE+1){
			updateWaveFreq(wId, MARK_FREQ); //Send stop bit.
			index = 0;
		}
		else{
			nextVal = popBit();
			index++;
			if( nextVal != currVal ){ /* Transition needed. */
				if(nextVal == LOGIC_1_VAL){
					updateWaveFreq(wId, MARK_FREQ);
				}
				else{ /* LOGIC_0_VAL */
					updateWaveFreq(wId, SPACE_FREQ);
				}
				currVal = nextVal;
			}
		}
	}
}

