/*
 * FTM.h
 *
 *  Created on: 16 oct. 2019
 *      Author: G5
 */

#ifndef FTM_H_
#define FTM_H_

#include <stdint.h>

#define NUMBER_OF_FTM_MODULES 4
#define FTM_MAX_PRESCALER 128
#define FTM_MIN_PRESCALER 1
#define FTM_DEFAULT_PRESCALER FTM_MIN_PRESCALER

typedef enum {FTM0_INDEX = 0, FTM1_INDEX = 1, FTM2_INDEX = 2, FTM3_INDEX = 3}FTMmodules;

typedef enum {FTM_CH0 = 0, FTM_CH1, FTM_N_CHANNELS, FTM_NO_CHANNEL}FTMchannels;

typedef enum {FTM_TIMER, FTM_INPUT_CAPTURE, FTM_OUTPUT_COMPARE, FTM_EPWM}FTMmodes;

typedef enum {FTM_PSCX1, FTM_PSCX2, FTM_PSCX4, FTM_PSCX8, FTM_PSCX16, FTM_PSCX32,
												FTM_PSCX64, FTM_PSCX128}FTMprescaler;

typedef enum {UP_COUNTER, UP_DOWN_COUNTER, FREE_COUNTER, QUAD_PHASE_COUNTER}FTM_TIMERcountModes;

typedef enum {UP_EDGE, UP_DOWN_EDGE, DOWN_EDGE}FTMedgeDetect;

typedef enum {FTM_HW_TRIGGER, FTM_SW_TRIGGER}FTMtrigger;

typedef enum {FTM_DMA_DISABLE, FTM_DMA_ENABLE}FTMdmaMode;

typedef void (*FTM_callback_t)(FTMchannels ch);

typedef struct{
	FTMmodules nModule; //FTM0_INDEX or FTM1_INDEX, etc

	FTMchannels nChannel; //FTM_CH0, FTM_CH1, etc.

	FTMmodes mode; //FTM_TIMER, FTM_INPUT_CAPTURE, etc

	FTMprescaler prescaler; //x1, x2, x4, etc

	FTM_TIMERcountModes countMode;

	FTMedgeDetect edge;

	FTMtrigger trigger;

	FTMdmaMode dmaMode;

	FTM_callback_t p2callback;   //pointer to subroutine called when FTM interrupts

	int numOverflows; //quantity of overflows before interrupt

	int nTicks; //count limit of FTM counter

	int CnV;

}FTMconfig_t;




void FTMinit(FTMconfig_t * p2config);

void updatePWMduty(FTMmodules id, FTMchannels ch, int dutyPercent);
int getPWMduty(FTMmodules id, FTMchannels ch);

void updatePWMperiod(FTMmodules id, FTMchannels ch, int newPeriodTime);

void updateCnV(FTMmodules id, FTMchannels ch, int newCnV);

int getCnV(FTMmodules id, FTMchannels ch);

int getMOD_FTM(FTMmodules id, FTMchannels ch);

int getCNTIN_FTM(FTMmodules id, FTMchannels ch);

void enableFTMinterrupts(FTMmodules id);

void disableFTMinterrupts(FTMmodules id);

void softwareFTMtrigger(FTMmodules id);

int shapeDuty2cnv(FTMmodules id, float dutyPercent);

void getFTMswTriggerREG(FTMmodules id, uint32_t * p2regSWtrigger, uint32_t * mask2SWtrigger);

int shapeFTMDifCaptured2Freq(FTMmodules id, int dif);

uint32_t * getCnVadress(FTMmodules id, FTMchannels ch);

#endif /* FTM_H_ */
