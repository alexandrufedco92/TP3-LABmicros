/*
 * waveGen.c
 *
 *  Created on: 23 oct. 2019
 *      Author: G5
 */


#include "waveGen.h"
#include "DAC.h"
#include "FTM.h"
#include <stdbool.h>
#include <math.h>
#include "timer.h"


#define IS_VALID_ID_WAVEGEN(x)  ((x >= 0) && (x < NUMBER_OF_WAVESGEN))

typedef struct{
	WAVEGENfreq freq;
	_Bool freqChangeRequest;
}waveGen_t;

waveGen_t wavesArray[NUMBER_OF_WAVESGEN];

int senLUT[N_SAMPLES];
int pwmSenLUT[N_SAMPLES];

void senoidalInit(void);
void sinWaveGen(WAVEGENid id, WAVEGENfreq freq);
void DACcallback(DACev);
void softwareTriggerDAC(void);
void pwmSinWaveGen(WAVEGENid id, WAVEGENfreq freq);
void FTMpwmCallback(FTMchannels ch);
void softwareTriggerFTM(void);



void initWaveGen(WaveGenConfig_t * p2config)
{
	if(IS_VALID_ID_WAVEGEN(p2config->id))
	{
		if((p2config->waveName == SIN_WAVEGEN))
		{
			//senoidalInit();  //Actually, this function only has to be called if it wasn't called yet.
			wavesArray[p2config->id].freq = p2config->freq;
			if((p2config->mode == SAMPLES_WAVEGEN))
			{
				sinWaveGen(p2config->id, p2config->freq);
			}
			else if((p2config->mode == PWM_WAVEGEN))
			{
				pwmSinWaveGen(p2config->id, p2config->freq);
			}
		}
	}

}

void updateWaveFreq(WAVEGENid id, WAVEGENfreq newFreq)
{
	if(IS_VALID_ID_WAVEGEN(id))
	{
		wavesArray[id].freqChangeRequest = true;
		wavesArray[id].freq = newFreq;
	}

}

WAVEGENfreq getWaveFreq(WAVEGENid id)
{
	return 1;
}

void stopWaveGen(WAVEGENid id)
{

}


void senoidalInit(void)
{
	float value = 0.00, senArg = 0.00, dutyPercent = 50.0;
	int i = 0;
	for(i = 0; i < N_SAMPLES; i++)
	{
		senArg = ((float) i) / N_SAMPLES;

		value = 1.65 + (1.65*sin(2*M_PI*senArg));
		senLUT[i] =  shapeValue2DACdata(value);

		dutyPercent = (value*100.0)/VIN_SELECTED;
		pwmSenLUT[i] = shapeDuty2cnv(FTM0_INDEX, dutyPercent);
	}
}

void sinWaveGen(WAVEGENid id, WAVEGENfreq freq)
{
	DACconfig_t DACconfig;
	DACconfig.dmaMode = DAC_DMA_DISABLE;
	DACconfig.mode = DAC_BUFFER_MODE;
	DACconfig.triggerMode = DAC_TRIGGER_SW;
	DACconfig.p2callback = DACcallback;

	DACinit(DAC0_ID, &DACconfig);
	InitializeTimers();
	SetTimer(SENOIDAL, (int)(1000.0/((float)(freq*N_SAMPLES))), softwareTriggerDAC);
}

void pwmSinWaveGen(WAVEGENid id, WAVEGENfreq freq)
{
	FTMconfig_t FTMpwmConfig;
	FTMpwmConfig.mode = FTM_EPWM;
	FTMpwmConfig.nModule = FTM0_INDEX;
	FTMpwmConfig.nChannel = FTM_CH0;
	FTMpwmConfig.countMode = UP_COUNTER;
	FTMpwmConfig.prescaler = FTM_PSCX32;
	FTMpwmConfig.CnV = 0;
	FTMpwmConfig.nTicks = 0xFFFF;
	FTMpwmConfig.numOverflows = 0;
	FTMpwmConfig.p2callback = FTMpwmCallback;
	FTMpwmConfig.dmaMode = FTM_DMA_DISABLE;
	FTMpwmConfig.trigger = FTM_SW_TRIGGER;

	FTMinit(&FTMpwmConfig);
	disableFTMinterrupts(FTMpwmConfig.nModule);
	senoidalInit();
	//enableFTMinterrupts(FTMpwmConfig.nModule);
	InitializeTimers();
	SetTimer(SENOIDAL_PWM, (int)(1000.0/((float)(freq*N_SAMPLES))), softwareTriggerFTM);


}
void DACcallback(DACev ev)
{
	static int i = 0, rpAux = 0;
	int j = 0, limit = 0;
	if(ev == DAC_RESET_RP_EV)
	{
		j = rpAux;
		limit = DAC_BUFFER_SIZE;
	}
	else if(ev == DAC_WM_RP_EV)
	{
		rpAux = DACgetBFreadPointer(DAC0_ID);
		j = 0;
		limit = rpAux;
	}
	while(j < limit) //fill the data buffer.
	{

		writeDACdata(DAC0_ID, j, senLUT[i]);
		i++;
		j++;
		if(i == N_SAMPLES)
		{
			i = 0;
		}
	}
}

void softwareTriggerDAC(void)
{
	updateSoftwareTrigger(DAC0_ID);
	if(wavesArray[WAVE0_WAVEGEN].freqChangeRequest)
	{
		DisableTimer(SENOIDAL);
		SetTimer(SENOIDAL, (int)(1000.0/((float)(wavesArray[WAVE0_WAVEGEN].freq*N_SAMPLES))), softwareTriggerDAC);
		wavesArray[WAVE0_WAVEGEN].freqChangeRequest = false;
	}
}
void softwareTriggerFTM(void)
{
	static int i = 0;
	updateCnV(FTM0_INDEX, FTM_CH0, pwmSenLUT[i]);
	softwareFTMtrigger(FTM0_INDEX);
	i++;
	if(i == N_SAMPLES)
	{
		i = 0;
	}
	if(wavesArray[WAVE0_WAVEGEN].freqChangeRequest)
	{
		DisableTimer(SENOIDAL_PWM);
		SetTimer(SENOIDAL_PWM, (int)(1000.0/((float)(wavesArray[WAVE0_WAVEGEN].freq*N_SAMPLES))), softwareTriggerDAC);
		wavesArray[WAVE0_WAVEGEN].freqChangeRequest = false;
	}
}
void FTMpwmCallback(FTMchannels ch)
{
	static int counter = 0;
	if(ch == FTM_NO_CHANNEL) //al final de cada periodo
	{
		counter++;
		if(counter == 5) //luego de 5 p
		{
			updatePWMperiod(FTM0_INDEX, FTM_CH0, 50); //50us
			updatePWMduty(FTM0_INDEX, FTM_CH0, 80); //80%
		}
		else if(counter == 10)
		{
			updatePWMperiod(FTM0_INDEX, FTM_CH0, 100); //100us
			updatePWMduty(FTM0_INDEX, FTM_CH0, 20); //20%
			counter = 0;
		}
	}
}
