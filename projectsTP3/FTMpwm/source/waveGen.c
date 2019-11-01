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
#include "PIT.h"
#include "DMA.h"
#include "MK64F12.h"


#define IS_VALID_ID_WAVEGEN(x)  ((x >= 0) && (x < NUMBER_OF_WAVESGEN))

typedef struct{
	WAVEGENfreq freq;
	int periodSignal;
	_Bool freqChangeRequest;
}waveGen_t;

waveGen_t wavesArray[NUMBER_OF_WAVESGEN];

int senLUT[N_SAMPLES];
int pwmSenLUT[N_SAMPLES];

void senoidalInit(WAVEGENmode wave);
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
			wavesArray[p2config->id].freq = p2config->freq;
			if((p2config->mode == SAMPLES_WAVEGEN))
			{
				senoidalInit(SAMPLES_WAVEGEN);
				sinWaveGen(p2config->id, p2config->freq);
				for (int i = 0; i < DAC_BUFFER_SIZE; i++)
				{
					writeDACdata(DAC0_ID, i, senLUT[i]);
				}
			}
			else if((p2config->mode == PWM_WAVEGEN))
			{
				pwmSinWaveGen(p2config->id, p2config->freq);
				senoidalInit(PWM_WAVEGEN);
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
		wavesArray[id].periodSignal = (int)(1000.0*(1000.0/((float)(newFreq*N_SAMPLES))));
	}

}

WAVEGENfreq getWaveFreq(WAVEGENid id)
{
	return 1;
}

void stopWaveGen(WAVEGENid id)
{

}


void senoidalInit(WAVEGENmode wave)
{
	float value = 0.00, senArg = 0.00, dutyPercent = 50.0;
	int i = 0;
	for(i = 0; i < N_SAMPLES; i++)
	{
		senArg = ((float) i) / N_SAMPLES;

		value = 1.65 + (1.65*sin(2*M_PI*senArg));
		senLUT[i] =  shapeValue2DACdata(value);

		if(wave == PWM_WAVEGEN)
		{
			dutyPercent = (value*100.0)/VIN_SELECTED;
			pwmSenLUT[i] = shapeDuty2cnv(FTM0_INDEX, dutyPercent);
		}
	}
}

void sinWaveGen(WAVEGENid id, WAVEGENfreq freq)
{
	DACconfig_t DACconfig;
	DACconfig.dmaMode = DAC_DMA_DISABLE;
	DACconfig.mode = DAC_BUFFER_MODE;
	DACconfig.triggerMode = DAC_TRIGGER_SW;
	DACconfig.p2callback = DACcallback;
	DACconfig.irqMode = DAC_IRQ_ENABLE;
	if(N_SAMPLES <= DAC_BUFFER_SIZE)  //buffer refreshment is not necessary
	{
		DACconfig.irqMode = DAC_IRQ_DISABLE;
	}

	DACinit(DAC0_ID, &DACconfig);

	float periodMs = 1000.0/((float)(freq*N_SAMPLES));
	config_t config = {	{(int)(periodMs*1000.0),0,0,0}, /* timerVal. */
							{true,false,false,false}, /* interruptEnable. */
							{true,false,false,false}, /* timerEnable. */
							{false,false,false,false}, /* chainMode. */
							{softwareTriggerDAC,NULL,NULL,NULL} }; /* pitCallbacks. */

	PITinit(&config);
}

void pwmSinWaveGen(WAVEGENid id, WAVEGENfreq freq)
{
	FTMconfig_t FTMpwmConfig, FTMpwmTriggerConfig;
	dma_transfer_conf_t conf;




	initDMA();
	//configureDMAMUX(DMA_WAVEGEN_CH, DMA_PIT1, true);
	conf.source_address = (uint32_t)pwmSenLUT;
	conf.dest_address = (uint32_t)getCnVadress(FTM0_INDEX, FTM_CH0);
	conf.offset = 0x02;
	conf.transf_size = BITS_16;
	conf.bytes_per_request = 0x02;	//paso 16bits=2bytes en cada dma request
	conf.total_bytes = conf.bytes_per_request*16;	//el total será 2bytes*16
	conf.mode = MEM_2_PERIPHERAL;
	conf.channel = DMA_WAVEGEN_CH;
	conf.dma_callback = NULL;
	conf.periodic_trigger = true;
	conf.request_source = DMA_PIT1;

	DMAPrepareTransfer(&conf);
	SIM->SOPT4 |=SIM_SOPT4_FTM0TRG0SRC(1);   //FTM1 triggers FTM trigger 0
	FTMpwmConfig.mode = FTM_EPWM;
	FTMpwmConfig.nModule = FTM0_INDEX;
	FTMpwmConfig.nChannel = FTM_CH0;
	FTMpwmConfig.countMode = UP_COUNTER;
	FTMpwmConfig.prescaler = FTM_PSCX4;
	FTMpwmConfig.CnV = 0;
	FTMpwmConfig.nTicks = 50;
	FTMpwmConfig.numOverflows = 0;
	FTMpwmConfig.p2callback = FTMpwmCallback;
	FTMpwmConfig.dmaMode = FTM_DMA_DISABLE;
	FTMpwmConfig.trigger = FTM_SW_TRIGGER;

	/*FTMpwmTriggerConfig.mode = FTM_OUTPUT_COMPARE;
	FTMpwmTriggerConfig.nModule = FTM1_INDEX;
	FTMpwmTriggerConfig.nChannel = FTM_CH1;
	FTMpwmTriggerConfig.countMode = UP_COUNTER;
	FTMpwmTriggerConfig.prescaler = FTM_PSCX4;
	FTMpwmTriggerConfig.CnV = 200;
	FTMpwmTriggerConfig.nTicks = 201;
	FTMpwmTriggerConfig.numOverflows = 0;
	FTMpwmTriggerConfig.p2callback = FTMpwmCallback;
	FTMpwmTriggerConfig.dmaMode = FTM_DMA_ENABLE;
	FTMpwmTriggerConfig.trigger = FTM_SW_TRIGGER;*/

	FTMinit(&FTMpwmConfig);
	FTMinit(&FTMpwmTriggerConfig);






	//disableFTMinterrupts(FTMpwmConfig.nModule);

	//enableFTMinterrupts(FTMpwmConfig.nModule);
	//float periodMs = 1000.0/((float)(freq*N_SAMPLES));
	float periodMs = 1000.0/((float)(freq*N_SAMPLES));
	int periodSampleUs = (int)(periodMs*1000.0);
	config_t config = {	{periodSampleUs/2, periodSampleUs,0,0},
								{true,false,false,false},
								{true,true,false,false},
								{false,false,false,false},
								{softwareTriggerFTM,NULL,NULL,NULL} };

	PITinit(&config);


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
		PITmodifyTimer(1, wavesArray[WAVE0_WAVEGEN].periodSignal);
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
		PITmodifyTimer(0, wavesArray[WAVE0_WAVEGEN].periodSignal/2);
		PITmodifyTimer(DMA_WAVEGEN_CH, wavesArray[WAVE0_WAVEGEN].periodSignal);
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
