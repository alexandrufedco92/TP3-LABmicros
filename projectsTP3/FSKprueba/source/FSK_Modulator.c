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
/*************************************************************
 * 				FUNCTIONS WITH GLOBAL SCOPE
 *************************************************************/
void ModulatorInit(void)
{
	#ifdef DAC_VERSION
		WaveGenConfig_t waveConf;
		waveConf.freq = 1200;
		waveConf.id = WAVE0_WAVEGEN;
		waveConf.mode = SAMPLES_WAVEGEN;
		waveConf.waveName = SIN_WAVEGEN;
		initWaveGen(&waveConf);
	#else	//PWM Version
		initFreqMeasure();
		WaveGenConfig_t waveConf;
		waveConf.freq = 1200;
		waveConf.id = WAVE0_WAVEGEN;
		waveConf.mode = PWM_WAVEGEN;
		waveConf.waveName = SIN_WAVEGEN;
		initWaveGen(&waveConf);
	#endif

}

