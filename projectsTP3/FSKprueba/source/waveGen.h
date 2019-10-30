/*
 * waveGen.h
 *
 *  Created on: 23 oct. 2019
 *      Author: G5
 */

#ifndef WAVEGEN_H_
#define WAVEGEN_H_

#define MAX_N_SAMPLES 1000 //Let's N_SAMPLES be under MAX_N_SAMPLES for correct functionality
#define N_SAMPLES 32

//MAX(WAVEGENfreq) = 1/(N_SAMPLES*min(Ttrigger).
//minTtrigger = 1ms --> MAX(WAVEGENfreq) = 1000/N_SAMPLES.

typedef enum{SIN_WAVEGEN, TRIANGULE_WAVEGEN}WAVEGENwaveName;

typedef enum{SAMPLES_WAVEGEN, PWM_WAVEGEN}WAVEGENmode;

typedef enum{WAVE0_WAVEGEN, WAVE1_WAVEGEN, WAVE2_WAVEGEN, NUMBER_OF_WAVESGEN}WAVEGENid;

typedef unsigned int WAVEGENfreq;   //Frequency in Hz

typedef struct{
	WAVEGENid id;
	WAVEGENwaveName waveName;
	WAVEGENmode mode;
	WAVEGENfreq freq;
}WaveGenConfig_t;

void initWaveGen(WaveGenConfig_t * p2config);

void updateWaveFreq(WAVEGENid id, WAVEGENfreq newFreq);

WAVEGENfreq getWaveFreq(WAVEGENid id);

void stopWaveGen(WAVEGENid id);




#endif /* WAVEGEN_H_ */
