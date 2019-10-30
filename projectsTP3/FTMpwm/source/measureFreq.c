/*
 * measureFreq.c
 *
 *  Created on: 25 oct. 2019
 *      Author: G5
 */

#include "measureFreq.h"
#include "FTM.h"
#include <stdbool.h>

#define SENSI_DIF 5 //ticks
#define DIF_CHANGE_DETECT(x, y) ((x >= y - SENSI_DIF) && (x <= y + SENSI_DIF))
#define TICK_FREQ 0.01

typedef struct{
	int freq; //Hz
	_Bool freqChanged;
	_Bool newMeasReady;
	int measuresLost;
}measureFreq_t;

int dif = 0;
float ticksScale = 1.0;
measureFreq_t measureDataBase;

void measFreqCallback(FTMchannels ch);

void initFreqMeasure(void)
{
	measureDataBase.newMeasReady = false;
	measureDataBase.freqChanged = false;
	measureDataBase.freq = 0;
	measureDataBase.measuresLost = 0;

	FTMconfig_t configInputCapture;
	configInputCapture.dmaMode = FTM_DMA_DISABLE;
	configInputCapture.mode = FTM_INPUT_CAPTURE;
	configInputCapture.edge = UP_EDGE;
	configInputCapture.countMode = UP_COUNTER;
	configInputCapture.nModule = FTM2_INDEX;
	configInputCapture.nChannel = FTM_CH0;
	configInputCapture.nTicks = 0xFFFF;
	configInputCapture.numOverflows = 0;
	configInputCapture.prescaler = FTM_PSCX32;
	configInputCapture.trigger = FTM_SW_TRIGGER;
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

			if(DIF_CHANGE_DETECT(dif, difAux))
			{
				measureDataBase.freqChanged = true;
				measureDataBase.freq = (int)((ticksScale/(float)dif)* 1000.0);
			}
			else
			{
				measureDataBase.freqChanged = false;
			}
			i = 0;
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
	}
	else //overflow
	{

	}


}

int getFreqMeasure(void)
{
	measureDataBase.newMeasReady = false;
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


