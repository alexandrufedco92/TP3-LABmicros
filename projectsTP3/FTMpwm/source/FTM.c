/*
 * FTM.c
 *
 *  Created on: 16 oct. 2019
 *      Author: G5
 */

#include "FTM.h"
//#include "pinsHandler.h"
#include "gpio.h"
#include <stdbool.h>


#define CANT_OF_MODES 5

#define FTM_IS_VALID_MODULE(x) ( (x >= 0) && (x < NUMBER_OF_FTM_MODULES) )
#define FTM_IS_VALID_CHANNEL(x) ( (x >= 0) && (x < FTM_N_CHANNELS) )

#define OUTPUT_COMPARE_TOGGLE_MODE (0x01)

#define F_CLOCK (50) //50 MHz

typedef enum {NO_CLOCK = 0, SYSTEM_CLOCK = 1, FIXED_CLOCK = 2, EXTERNAL_CLOCK = 3}clocksSource;

FTM_Type * arrayP2FTM[] = FTM_BASE_PTRS;
IRQn_Type arrayFTMirqs[] = FTM_IRQS;
FTM_callback_t arrayFTMcallbacks[NUMBER_OF_FTM_MODULES];

void FTMclockGating(void);
void setFTMprescaler(FTMprescaler psc, FTM_Type * p2FTM);
void setFTMtimer(FTMmodules nModule, FTM_TIMERcountModes countMode, uint16_t nTicks, FTM_callback_t p2callback);
void enablePinFTM(FTMmodules id, FTMchannels ch);
uint8_t getPrescalerFactor(FTM_Type * p2FTM);

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
	if(FTM_IS_VALID_MODULE(p2config->nModule))
	{
		p2FTM = arrayP2FTM[p2config->nModule];
		p2FTM->SC &= ~((uint32_t)FTM_SC_CLKS_MASK); //deshabilito para permitir configurar
		enablePinFTM(p2config->nModule, p2config->nChannel);
		//2)
		p2FTM->MODE |= FTM_MODE_FTMEN(1);

		//4)
		setFTMprescaler(p2config->prescaler, p2FTM);
		//5)
		p2FTM->CONF &= ~((uint32_t)FTM_CONF_NUMTOF_MASK);
		p2FTM->CONF |= FTM_CONF_NUMTOF(p2config->numOverflows);

		if(p2config->mode == FTM_TIMER)
		{
			setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
		}
		else if(p2config->mode == FTM_OUTPUT_COMPARE)
		{
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
			SIM->SOPT4 |= SIM_SOPT4_FTM2CH0SRC(1);
			setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
			p2FTM->SC &= ~FTM_SC_CPWMS_MASK;
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_ELSA_MASK) & (~FTM_CnSC_ELSB_MASK);
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_MSA_MASK) & (~FTM_CnSC_MSB_MASK);
			p2FTM->COMBINE &= (~FTM_COMBINE_COMP0_MASK) & (~FTM_COMBINE_DECAPEN0_MASK);
			if(p2config->edge == UP_EDGE)
			{
				(p2FTM->CONTROLS[p2config->nChannel]).CnSC |= FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1);
			}
			else if(p2config->edge == UP_DOWN_EDGE)
			{
				(p2FTM->CONTROLS[p2config->nChannel]).CnSC |= FTM_CnSC_ELSB(1) | FTM_CnSC_ELSA(1);
			}
			else if(p2config->edge == DOWN_EDGE)
			{
				(p2FTM->CONTROLS[p2config->nChannel]).CnSC |= FTM_CnSC_ELSB(1) | FTM_CnSC_ELSA(0);
			}
			p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_CHIE(1);
		}
		else if(p2config->mode == FTM_EPWM)
		{
			if(p2config->dmaMode == FTM_DMA_DISABLE)
			{
				setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
				p2FTM->SC &= ~FTM_SC_CPWMS_MASK;
				(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_ELSA_MASK) & (~FTM_CnSC_ELSB_MASK);
				(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_MSA_MASK) & (~FTM_CnSC_MSB_MASK);
				p2FTM->COMBINE &= (~FTM_COMBINE_COMP0_MASK) & (~FTM_COMBINE_DECAPEN0_MASK);

				(p2FTM->CONTROLS[p2config->nChannel]).CnSC |= (FTM_CnSC_MSB_MASK) | FTM_CnSC_ELSB_MASK;
				p2FTM->CONTROLS[p2config->nChannel].CnV = ((p2FTM->MOD & FTM_MOD_MOD_MASK)/2);
				updatePWMduty(p2config->nModule, p2config->nChannel, 20);
				updatePWMperiod(p2config->nModule, p2config->nChannel, 100);

				//SYNCHRONIZATION
				p2FTM->MODE |= FTM_MODE_PWMSYNC(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_SYNCMODE(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_CNTINC(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_SWWRBUF(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_SWRSTCNT(1);

				p2FTM->COMBINE |= FTM_COMBINE_SYNCEN0(1);

				p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);

							/////
				p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_CHIE(1);
			}
			else if(p2config->dmaMode == FTM_DMA_ENABLE)
			{

			}


		}
		//3)
		p2FTM->SC &= ~((uint32_t)FTM_SC_CLKS_MASK);
		p2FTM->SC |= FTM_SC_CLKS(SYSTEM_CLOCK);
	}

}

void setFTMtimer(FTMmodules nModule, FTM_TIMERcountModes countMode, uint16_t nTicks, FTM_callback_t p2callback)
{
	FTM_Type * p2FTM;
	if(FTM_IS_VALID_MODULE(nModule) && (nTicks > 0))
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

void updatePWMduty(FTMmodules id, FTMchannels ch, int dutyPercent)
{
	FTM_Type * p2FTM;
	uint32_t nTicksPeriod = 1;
	if(FTM_IS_VALID_MODULE(id) && FTM_IS_VALID_CHANNEL(ch) && (dutyPercent > 0))
	{
		p2FTM = arrayP2FTM[id];
		p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
		nTicksPeriod = (p2FTM->MOD & FTM_MOD_MOD_MASK) - (p2FTM->CNTIN & FTM_CNTIN_INIT_MASK);
		p2FTM->CONTROLS[ch].CnV = (p2FTM->CNTIN & FTM_CNTIN_INIT_MASK) + (uint32_t)((nTicksPeriod*dutyPercent)/100);
		p2FTM->SYNC |= FTM_SYNC_SWSYNC(1);
		//p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
	}
}
int shapeDuty2cnv(FTMmodules id, float dutyPercent)
{
	FTM_Type * p2FTM;
	uint32_t nTicksPeriod = 1;
	int ret = 0;
	if(FTM_IS_VALID_MODULE(id))
	{
		p2FTM = arrayP2FTM[id];
		nTicksPeriod = (p2FTM->MOD & FTM_MOD_MOD_MASK) - (p2FTM->CNTIN & FTM_CNTIN_INIT_MASK);
		ret = (p2FTM->CNTIN & FTM_CNTIN_INIT_MASK) + (uint32_t)((((float)nTicksPeriod)*dutyPercent)/100.0);
	}
	return ret;
}
int getPWMduty(FTMmodules id, FTMchannels ch)
{
	FTM_Type * p2FTM;
	uint32_t nTicksPeriod = 1;
	int dutyPercent = 50;
	if(FTM_IS_VALID_MODULE(id) && FTM_IS_VALID_CHANNEL(ch))
	{
		p2FTM = arrayP2FTM[id];
		nTicksPeriod = (p2FTM->MOD & FTM_MOD_MOD_MASK) - (p2FTM->CNTIN & FTM_CNTIN_INIT_MASK);
		dutyPercent = ((p2FTM->CONTROLS[ch].CnV - (p2FTM->CNTIN & FTM_CNTIN_INIT_MASK))*100)/nTicksPeriod;
	}
	return dutyPercent;
}

void updatePWMperiod(FTMmodules id, FTMchannels ch, int newPeriodTime)
{
	FTM_Type * p2FTM;
	uint8_t prescalerFactor = 1;
	uint32_t nTicks = 1;
	int dutyAux = 50;
	if(FTM_IS_VALID_MODULE(id) && FTM_IS_VALID_CHANNEL(ch))
	{
		p2FTM = arrayP2FTM[id];
		p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
		dutyAux = getPWMduty(id, ch);
		p2FTM->CNTIN &= ~((uint32_t)FTM_CNTIN_INIT_MASK);
		p2FTM->MOD &= ~((uint32_t)FTM_MOD_MOD_MASK);

		prescalerFactor = getPrescalerFactor(p2FTM);
		nTicks = (((uint32_t)newPeriodTime)*F_CLOCK)/(prescalerFactor);
		p2FTM->MOD |= FTM_MOD_MOD(nTicks - 1);

		//p2FTM->SYNC |= FTM_SYNC_SWSYNC(1);
		//p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
		updatePWMduty(id, ch, dutyAux);
	}
}


void updateCnV(FTMmodules id, FTMchannels ch, int newCnV)
{
	FTM_Type * p2FTM;
	if(FTM_IS_VALID_MODULE(id) && FTM_IS_VALID_CHANNEL(ch))
	{
		p2FTM = arrayP2FTM[id];
		p2FTM->CONTROLS[ch].CnV = newCnV;
	}
}

int getCnV(FTMmodules id, FTMchannels ch)
{
	int ret = 0;
	FTM_Type * p2FTM;
	if(FTM_IS_VALID_MODULE(id) && FTM_IS_VALID_CHANNEL(ch))
	{
		p2FTM = arrayP2FTM[id];
		ret = p2FTM->CONTROLS[ch].CnV;
	}
	return ret;
}
int getMOD_FTM(FTMmodules id, FTMchannels ch)
{
	int ret = 0;
	FTM_Type * p2FTM;
	if(FTM_IS_VALID_MODULE(id) && FTM_IS_VALID_CHANNEL(ch))
	{
		p2FTM = arrayP2FTM[id];
		ret = p2FTM->MOD;
	}
	return ret;
}

int getCNTIN_FTM(FTMmodules id, FTMchannels ch)
{
	int ret = 0;
	FTM_Type * p2FTM;
	if(FTM_IS_VALID_MODULE(id) && FTM_IS_VALID_CHANNEL(ch))
	{
		p2FTM = arrayP2FTM[id];
		ret = p2FTM->CNTIN;
	}
	return ret;
}

void enableFTMinterrupts(FTMmodules id)
{
	NVIC_EnableIRQ(arrayFTMirqs[id]);
}

void disableFTMinterrupts(FTMmodules id)
{
	NVIC_DisableIRQ(arrayFTMirqs[id]);
}

void enablePinFTM(FTMmodules id, FTMchannels ch)  //This function has to be enhanced.
{
	if((id == FTM0_INDEX) && (ch == FTM_CH0))  //PC1 (ALT 4)
	{
		setPCRmux(PORTC, 1, 4);
	}
	else if((id == FTM2_INDEX) && (ch == FTM_CH0)) //PC9 (ALT 3)
	{
		setPCRmux(PORTB, 18, 3);
	}
}

uint8_t getPrescalerFactor(FTM_Type * p2FTM)
{
	uint8_t prescalerFactor = 1;
	switch(p2FTM->SC & FTM_SC_PS_MASK)
	{
		case FTM_PSCX1:
			prescalerFactor = 1;
			break;
		case FTM_PSCX2:
			prescalerFactor = 2;
			break;
		case FTM_PSCX4:
			prescalerFactor = 4;
			break;
		case FTM_PSCX8:
			prescalerFactor = 8;
			break;
		case FTM_PSCX16:
			prescalerFactor = 16;
			break;
		case FTM_PSCX32:
			prescalerFactor = 32;
			break;
		case FTM_PSCX64:
			prescalerFactor = 64;
			break;
		case FTM_PSCX128:
			prescalerFactor = 128;
			break;
	}
	return prescalerFactor;
}
void softwareFTMtrigger(FTMmodules id)
{
	FTM_Type * p2FTM;
	if(FTM_IS_VALID_MODULE(id))
	{
		p2FTM = arrayP2FTM[id];
		//p2FTM->CONTROLS[ch].CnV = newCnV;
		p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
		p2FTM->SYNC |= FTM_SYNC_SWSYNC(1);
	}

}
void FTMx_IRQHandler(FTMmodules nModule)
{
	FTM_Type * p2FTM = arrayP2FTM[nModule];
	_Bool isChannelInterrupt = false;
	int i = 0;
	for(i = 0; (i < FTM_N_CHANNELS) && (!isChannelInterrupt); i++)	//is some channel's event.
	{
		if(((p2FTM->CONTROLS[i]).CnSC) & (FTM_CnSC_CHF_MASK))
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
