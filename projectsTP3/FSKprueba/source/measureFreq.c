/*
 * measureFreq.c
 *
 *  Created on: 25 oct. 2019
 *      Author: G5
 */

#include "measureFreq.h"
#include "FTM.h"
#include "CMP.h"
#include "DMA.h"
#include <stdbool.h>

#define MIN_DIF 50
#define SENSI_DIF 5 //ticks
#define NO_GLITCH(x) (x > MIN_DIF)
#define DIF_CHANGE_DETECT(x, y) ((x < y - SENSI_DIF) || (x > y + SENSI_DIF))
#define TICK_FREQ 0.01

typedef struct{
	int freq; //Hz
	_Bool freqChanged;
	_Bool newMeasReady;
	int measuresLost;
}measureFreq_t;

uint16_t capturesWithDMA[2];

int dif = 0;
float ticksScale = 1.0;
measureFreq_t measureDataBase;

void dmaCaptureCallback(void);

void measFreqCallback(FTMchannels ch);

void initFreqMeasure(void)
{
	measureDataBase.newMeasReady = false;
	measureDataBase.freqChanged = false;
	measureDataBase.freq = 0;
	measureDataBase.measuresLost = 0;

	initCMP(CMP_0);
	//initDMA();

	//dma_transfer_conf_t conf;

	//configureDMAMUX(DMA_EXAMPLE, DMA_FTMX_CHX, false);
	/*conf.source_address = (uint32_t)getCnVadress(FTM2_INDEX, FTM_CH0);      //al registro
	conf.dest_address = (uint32_t)capturesWithDMA;     //al arreglo que tiene para poner los dos valores
	conf.offset = 0x02;         //paso dos bytes (=16 bits)
	conf.transf_size = BITS_16;
	conf.bytes_per_request = 0x02;  //2 bytes
	conf.total_bytes = 0x04;            //el total es 2bytes*2
	conf.mode = PERIPHERAL_2_MEM;
	conf.channel = DMA_CAPTURE_CH;
	conf.dma_callback = dmaCaptureCallback;
	conf.periodic_trigger = false;
	conf.request_source = DMA_FTM2_CH0;*/
	//DMAPrepareTransfer(&conf);

	FTMconfig_t configInputCapture;
	//configInputCapture.dmaMode = FTM_DMA_ENABLE;
	configInputCapture.dmaMode = FTM_DMA_DISABLE;
	configInputCapture.mode = FTM_INPUT_CAPTURE;
	configInputCapture.edge = UP_DOWN_EDGE;
	configInputCapture.countMode = UP_COUNTER;
	configInputCapture.nModule = FTM2_INDEX;
	configInputCapture.nChannel = FTM_CH0;
	configInputCapture.nTicks = 0xFFFF;
	configInputCapture.numOverflows = 0;
	configInputCapture.prescaler = FTM_PSCX32;
	//configInputCapture.trigger = FTM_SW_TRIGGER;
	configInputCapture.trigger = FTM_HW_TRIGGER;
	configInputCapture.p2callback = measFreqCallback;

	ticksScale = 50000.0/32.0;
	FTMinit(&configInputCapture);

}

void measFreqCallback(FTMchannels ch)
{
	static int i = 0;
	static int firstMeasure = 0;
	static int secondMeasure = 0;
	static int difAux = 0;
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
			if(secondMeasure < firstMeasure)  //overflow
			{
				firstMeasure = firstMeasure - getMOD_FTM(FTM2_INDEX, FTM_CH0);
			}
			dif = secondMeasure - firstMeasure;
			firstMeasure = secondMeasure;
			i = 1;
			if(NO_GLITCH(dif) && DIF_CHANGE_DETECT(dif, difAux))
			{
				measureDataBase.freqChanged = true;
				//measureDataBase.freq = (int)((ticksScale/(float)dif)* 1000.0);
			}
			else
			{
				measureDataBase.freqChanged = false;
			}
			difAux = dif;
			//i = 0;
			if(measureDataBase.newMeasReady)
			{
				measureDataBase.measuresLost++;
			}
			else
			{
				measureDataBase.newMeasReady = true;
				measureDataBase.measuresLost = 0;
			}
		}
	}
	else //overflow
	{

	}


}

int getFreqMeasure(void)
{
	measureDataBase.newMeasReady = false;
	measureDataBase.freq = (int)((ticksScale/(float)dif)* 500.0);
	return measureDataBase.freq;
}

_Bool isNewMeasReady(void)
{
	return measureDataBase.newMeasReady;
}

_Bool freqHasChanged(void)
{
	return measureDataBase.freqChanged;
}

void dmaCaptureCallback(void)
{
	static int difAux = 0;
	int dif;

	if(capturesWithDMA[1] < capturesWithDMA[0])
	{
		capturesWithDMA[0] = capturesWithDMA[0] - getMOD_FTM(FTM2_INDEX, FTM_CH0);
	}
	dif = capturesWithDMA[1] - capturesWithDMA[0];

	if(NO_GLITCH(dif) && DIF_CHANGE_DETECT(dif, difAux))
	{
		measureDataBase.freqChanged = true;
		//measureDataBase.freq = (int)((ticksScale/(float)dif)* 1000.0);
	}
	else
	{
		measureDataBase.freqChanged = false;
	}

	difAux = dif;

	if(measureDataBase.newMeasReady)
	{
		measureDataBase.measuresLost++;
	}
	else
	{
		measureDataBase.newMeasReady = true;
		measureDataBase.measuresLost = 0;
	}
}





