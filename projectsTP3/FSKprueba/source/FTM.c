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
	gpioMode(PORTNUM2PIN(PD, 1), OUTPUT);
	FTM_Type * p2FTM;
	//1)
	FTMclockGating();
	if(FTM_IS_VALID_MODULE(p2config->nModule))
	{
		p2FTM = arrayP2FTM[p2config->nModule];
		p2FTM->SC &= ~((uint32_t)FTM_SC_CLKS_MASK); //disable this to allow the configuration
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
			p2FTM->EXTTRIG |= FTM_EXTTRIG_CH1TRIG(1);
			SIM->SOPT4 |=SIM_SOPT4_FTM0TRG0SRC(1);   //FTM1 triggers FTM trigger 0
			p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_CHIE(1);
			p2FTM->SC |=FTM_SC_TOIE(1);
			//NVIC_EnableIRQ(arrayFTMirqs[p2config->nModule]);
			//p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_DMA(1);


		}
		else if(p2config->mode == FTM_OUTPUT_COMPARE)
		{
			setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_ELSA_MASK) & (~FTM_CnSC_ELSB_MASK);
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC |= FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1);
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_MSA_MASK) & (~FTM_CnSC_MSB_MASK);
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC |= FTM_CnSC_MSB(0) | FTM_CnSC_MSA(1);
			p2FTM->COMBINE &= (~FTM_COMBINE_COMP0_MASK) & (~FTM_COMBINE_DECAPEN0_MASK);





			p2FTM->SC |=FTM_SC_TOIE(1);
			p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_CHIE(1);
			//NVIC_EnableIRQ(arrayFTMirqs[p2config->nModule]);
			p2FTM->COMBINE |= FTM_COMBINE_SYNCEN0(1);
			SIM->SOPT4 &= ~SIM_SOPT4_FTM0TRG0SRC_MASK;
			asm("nop");
			asm("nop");
			SIM->SOPT4 |=SIM_SOPT4_FTM0TRG0SRC(1);
			p2FTM->EXTTRIG |= FTM_EXTTRIG_INITTRIGEN(1);

		}
		else if(p2config->mode == FTM_INPUT_CAPTURE)
		{

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


			//p2FTM->SC |=FTM_SC_TOIE(1);
			p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_CHIE(1);


			if(p2config->dmaMode == FTM_DMA_DISABLE)
			{
				p2FTM->CONTROLS[p2config->nChannel].CnSC &= ~FTM_CnSC_DMA_MASK;
				NVIC_EnableIRQ(arrayFTMirqs[p2config->nModule]);
			}
			else if(p2config->dmaMode == FTM_DMA_ENABLE)
			{
				p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_DMA(1);
				//NVIC_EnableIRQ(arrayFTMirqs[p2config->nModule]);
			}
			if(p2config->trigger == FTM_HW_TRIGGER)
			{
				//config para hardware trigger
				SIM->SOPT4 |= SIM_SOPT4_FTM2CH0SRC(1);
			}


		}
		else if(p2config->mode == FTM_EPWM)
		{
			setFTMtimer(p2config->nModule, p2config->countMode, (uint16_t)(p2config->nTicks), p2config->p2callback);
			p2FTM->SC &= ~FTM_SC_CPWMS_MASK;
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_ELSA_MASK) & (~FTM_CnSC_ELSB_MASK);
			(p2FTM->CONTROLS[p2config->nChannel]).CnSC &= (~FTM_CnSC_MSA_MASK) & (~FTM_CnSC_MSB_MASK);
			p2FTM->COMBINE &= (~FTM_COMBINE_COMP0_MASK) & (~FTM_COMBINE_DECAPEN0_MASK);
			p2FTM->CONTROLS[p2config->nChannel].CnSC &= ~FTM_CnSC_DMA_MASK;

			(p2FTM->CONTROLS[p2config->nChannel]).CnSC |= (FTM_CnSC_MSB_MASK) | FTM_CnSC_ELSB_MASK;
			p2FTM->CONTROLS[p2config->nChannel].CnV = ((p2FTM->MOD & FTM_MOD_MOD_MASK)/2);
			if(p2config->dmaMode == FTM_DMA_DISABLE)
			{
				p2FTM->CONTROLS[p2config->nChannel].CnSC &= ~FTM_CnSC_DMA_MASK;
			}
			else if(p2config->dmaMode == FTM_DMA_ENABLE)
			{
				p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_CHIE(1);
				p2FTM->CONTROLS[p2config->nChannel].CnSC |= FTM_CnSC_DMA(1);
			}


			//SYNCHRONIZATION
			p2FTM->MODE |= FTM_MODE_PWMSYNC(1);
			p2FTM->SYNCONF |= FTM_SYNCONF_SYNCMODE(1);
			p2FTM->SYNCONF |= FTM_SYNCONF_CNTINC(1);
			if(p2config->trigger == FTM_SW_TRIGGER)
			{
				p2FTM->MODE &= ~FTM_MODE_PWMSYNC_MASK;
				p2FTM->FMS &= ~FTM_FMS_WPEN_MASK;
				p2FTM->MODE |= FTM_MODE_INIT(1) | FTM_MODE_WPDIS(1) | FTM_MODE_FTMEN(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_SWWRBUF(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_SWOM_MASK; //enable mask function
				p2FTM->SYNC |= (FTM_SYNC_SWSYNC_MASK|FTM_SYNC_CNTMIN_MASK);
				p2FTM->SYNCONF &= ~FTM_SYNCONF_SWRSTCNT_MASK;
			}
			else if(p2config->trigger == FTM_HW_TRIGGER)
			{
				p2FTM->SYNC |= FTM_SYNC_TRIG0(1);
				p2FTM->MODE &= ~FTM_MODE_PWMSYNC_MASK;
				p2FTM->FMS &= ~FTM_FMS_WPEN_MASK;
				p2FTM->MODE &= ~FTM_MODE_PWMSYNC_MASK;
				p2FTM->MODE |= FTM_MODE_INIT(1) | FTM_MODE_WPDIS(1) | FTM_MODE_FTMEN(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_HWWRBUF(1) | FTM_SYNCONF_HWSOC(1);
				p2FTM->SYNCONF |= FTM_SYNCONF_SWOM_MASK; //enable mask function
				p2FTM->SYNCONF &= ~FTM_SYNCONF_HWRSTCNT_MASK;
				//p2FTM->SYNCONF |= FTM_SYNCONF_HWRSTCNT(1);

			}



			p2FTM->COMBINE |= FTM_COMBINE_SYNCEN0(1);

			//p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
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
	arrayP2FTM[id]->CONTROLS[ch].CnV = newCnV;
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

void getFTMswTriggerREG(FTMmodules id, uint32_t * p2regSWtrigger, uint32_t * mask2SWtrigger)
{
	FTM_Type* p2FTM = arrayP2FTM[id];
	p2regSWtrigger = &(p2FTM->SYNC);
	*mask2SWtrigger = p2FTM->SYNC | FTM_SYNC_SWSYNC_MASK;
}

uint32_t * getCnVadress(FTMmodules id, FTMchannels ch)
{
	FTM_Type* p2FTM = arrayP2FTM[id];
	return &(p2FTM->CONTROLS[ch].CnV);
}

uint32_t * getSYNCadress(FTMmodules id, FTMchannels ch)
{
	FTM_Type* p2FTM = arrayP2FTM[id];
	return &(p2FTM->SYNC);
}

int shapeFTMDifCaptured2Freq(FTMmodules id, int dif)
{
	FTM_Type* p2FTM = arrayP2FTM[id];
	int ticksScale = 50000.0/((float)getPrescalerFactor(p2FTM));
	return (int)((ticksScale/(float)dif)* 1000.0);
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
	else if((id == FTM1_INDEX) && (ch == FTM_CH1))
	{
		setPCRmux(PORTA, 13, 3);
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
		//p2FTM->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
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
		gpioWrite(PORTNUM2PIN(PD,1),true);
		arrayFTMcallbacks[nModule](FTM_NO_CHANNEL); //It calls the callback of the indicated module.
		arrayP2FTM[nModule]->SC &= ~FTM_SC_TOF_MASK;   //reset the flag that indicates interrupt
		gpioWrite(PORTNUM2PIN(PD,1),false);
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
