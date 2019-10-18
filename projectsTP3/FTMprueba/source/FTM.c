/*
 * FTM.c
 *
 *  Created on: 16 oct. 2019
 *      Author: G5
 */

#include "FTM.h"
#include "pinsHandler.h"
#include <stdbool.h>

#define CANT_OF_MODES 5

#define FTM_IS_VALID_MODULE(x) ( (x >= 0) && (x < NUMBER_OF_FTM_MODULES) )

#define OUTPUT_COMPARE_TOGGLE_MODE (0x01)

typedef enum {NO_CLOCK = 0, SYSTEM_CLOCK = 1, FIXED_CLOCK = 2, EXTERNAL_CLOCK = 3}clocksSource;

FTM_Type * arrayP2FTM[] = FTM_BASE_PTRS;
IRQn_Type arrayFTMirqs[] = FTM_IRQS;
FTM_callback_t arrayFTMcallbacks[NUMBER_OF_FTM_MODULES];

void FTMclockGating(void);
void setFTMprescaler(FTMprescaler psc, FTM_Type * p2FTM);
void setFTMtimer(FTMmodules nModule, FTM_TIMERcountModes countMode, uint16_t nTicks, FTM_callback_t p2callback);

void FTMx_IRQHandler(FTMmodules nModule);
void FTM0_IRQHandler(void);
void FTM1_IRQHandler(void);
void FTM2_IRQHandler(void);

/*Steps to correct initialization:
 * 1) Clock gating
 * 2) FTM advanced function (FTMx_MODE->FTMEN=1)
 * 3) Choose clock source FTMx_SC->CLCKS = CLOCK_DESIRED
 * 4) Set prescaler
 * 5) Set NUMTOF
 * 6) Identify mode and do the subroutine for that one.
 * */
void FTMinit(FTMconfig_t * p2config)
{
	FTM_Type * p2FTM;
	//1)
	FTMclockGating();
	if(FTM_IS__VALID_MODULE(p2config->nModule))
	{
		p2FTM = arrayP2FTM[p2config->nModule];
		//2)
		p2FTM->MODE |= FTM_MODE_FTMEN(1);
		//3)
		p2FTM->SC &= ~((uint32_t)FTM_SC_CLKS_MASK);
		p2FTM->SC |= FTM_SC_CLKS(SYSTEM_CLOCK);
		//4)
		setFTMprescaler(p2config->prescaler, p2FTM);
		//5)
		p2FTM->CONF &= ~((uint32_t)FTM_CONF_NUMTOF_MASK);
		p2FTM->CONF |= FTM_CONF_NUMTOF(p2config->numOverflows);

		if(p2config->mode == FTM_TIMER)
		{
			////MUXXXXXX
			setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
		}
		else if(p2config->mode == FTM_OUTPUT_COMPARE)
		{
			//MUXXXXXX FTM0_CH0
			setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
			(p2FTM->CONTROLS[FTM_CH0]).CnSC &= (~FTM_CnSC_ELSA_MASK) & (~FTM_CnSC_ELSB_MASK);
			(p2FTM->CONTROLS[FTM_CH0]).CnSC |= FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1);
			(p2FTM->CONTROLS[FTM_CH0]).CnSC &= (~FTM_CnSC_MSA_MASK) & (~FTM_CnSC_MSB_MASK);
			(p2FTM->CONTROLS[FTM_CH0]).CnSC |= FTM_CnSC_MSB(0) | FTM_CnSC_MSA(1);
			p2FTM->COMBINE &= (~FTM_COMBINE_COMP0_MASK) & (~FTM_COMBINE_DECAPEN0_MASK);
			p2FTM->CONTROLS[FTM_CH0].CnSC |= FTM_CnSC_CHIE(1);

		}
		else if(p2config->mode == FTM_INPUT_CAPTURE)
		{
			////MUXXXX FTM3_CH5
			setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
			p2FTM->SC &= ~FTM_SC_CPWMS_MASK;
			(p2FTM->CONTROLS[FTM_CH5]).CnSC &= (~FTM_CnSC_ELSA_MASK) & (~FTM_CnSC_ELSB_MASK);
			(p2FTM->CONTROLS[FTM_CH5]).CnSC |= FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1);
			(p2FTM->CONTROLS[FTM_CH5]).CnSC &= (~FTM_CnSC_MSA_MASK) & (~FTM_CnSC_MSB_MASK);
			(p2FTM->CONTROLS[FTM_CH5]).CnSC |= FTM_CnSC_MSB(0) | FTM_CnSC_MSA(1);
			p2FTM->COMBINE &= (~FTM_COMBINE_COMP0_MASK) & (~FTM_COMBINE_DECAPEN0_MASK);
			p2FTM->CONTROLS[FTM_CH5].CnSC |= FTM_CnSC_CHIE(1);
		}
	}

}

void setFTMtimer(FTMmodules nModule, FTM_TIMERcountModes countMode, uint16_t nTicks, FTM_callback_t p2callback)
{
	FTM_Type * p2FTM;
	if(FTM_IS__VALID_MODULE(nModule) && (nTicks > 0))
	{
		p2FTM = arrayP2FTM[nModule];
		p2FTM->QDCTRL &= ~((uint32_t)FTM_QDCTRL_QUADEN_MASK);
		p2FTM->SC &= ~((uint32_t)FTM_SC_CPWMS_MASK);
		p2FTM->CNTIN &= ~((uint32_t)FTM_CNTIN_INIT_MASK);
		p2FTM->MOD &= ~((uint32_t)FTM_MOD_MOD_MASK);
		p2FTM->SC &= ~((uint32_t)FTM_SC_TOIE_MASK);
		p2FTM->CNTIN |= FTM_CNTIN_INIT(0);
		if(countMode == UP_COUNTER)	//The flags QUADEN and CPWMS are in zeros.
		{
			p2FTM->MOD |= FTM_MOD_MOD(nTicks - 1);
		}
		else if(countMode == UP_DOWN_COUNTER)
		{
			p2FTM->SC |= FTM_SC_CPWMS(1);
			p2FTM->MOD |= FTM_MOD_MOD(nTicks);
		}
		else if(countMode == FREE_COUNTER)
		{
			p2FTM->MOD &= FTM_MOD_MOD_MASK;
		}
		arrayFTMcallbacks[nModule] = p2callback;
		p2FTM->SC |=FTM_SC_TOIE(1);
		NVIC_EnableIRQ(arrayFTMirqs[nModule]);
	}

}


void FTMclockGating(void)
{
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
}

void setFTMprescaler(FTMprescaler psc, FTM_Type * p2FTM)
{
	p2FTM->SC &= ~((uint32_t)FTM_SC_PS_MASK);
	p2FTM->SC |= FTM_SC_PS(psc);
}

void updatePWMduty(FTMmodules id, FTMchannels ch, uint8_t dutyPercent)
{

}

void updatePWMperiod(FTMmodules id, FTMchannels ch, uint32_t newPeriodTime)
{

}


void updateCnV(FTMmodules id, FTMchannels ch, uint32_t newCnV)
{

}

int getCnV(FTMmodules id, FTMchannels ch)
{
	return 0;
}

void enableFTMinterrupts(FTMmodules id)
{
	NVIC_EnableIRQ(arrayFTMirqs[id]);
}

void disableFTMinterrupts(FTMmodules id)
{
	NVIC_DisableIRQ(arrayFTMirqs[id]);
}

void FTMx_IRQHandler(FTMmodules nModule)
{
	FTM_Type * p2FTM = arrayP2FTM[nModule];
	_Bool isChannelInterrupt = false;
	int i = 0;
	for(i = 0; (i < FTM_N_CHANNELS) && (!isChannelInterrupt); i++)	//is some channel's event.
	{
		if(p2FTM->CONTROLS[i].CnSC & FTM_CnSC_CHF_MASK)
		{
			isChannelInterrupt = true;
		}
	}
	if(isChannelInterrupt)
	{
		arrayFTMcallbacks[nModule](i - 1);
		p2FTM->CONTROLS[i - 1].CnSC &=  ~FTM_CnSC_CHF_MASK;  //reset flag
	}
	else if(arrayP2FTM[nModule]->SC & FTM_SC_TOF_MASK) //if the count is finished
	{
		arrayFTMcallbacks[nModule](FTM_NO_CHANNEL); //It calls the callback of the indicated module.
		arrayP2FTM[nModule]->SC &= ~FTM_SC_TOF_MASK;   //reset the flag that indicates interrupt
	}
}
void FTM0_IRQHandler(void)
{
	FTMx_IRQHandler(FTM0_INDEX);
}
void FTM1_IRQHandler(void)
{
	FTMx_IRQHandler(FTM1_INDEX);
}
void FTM2_IRQHandler(void)
{
	FTMx_IRQHandler(FTM2_INDEX);
}
