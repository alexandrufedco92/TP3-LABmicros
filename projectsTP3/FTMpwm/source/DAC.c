/*
 * DAC.c
 *
 *  Created on: 17 oct. 2019
 *      Author: G5
 */

#include "DAC.h"
//#include "pinsHandler.h"
#include "gpio.h"




#define VIN_SELECTED 3.3

#define IS_VALID_ID_DAC(x) ( (x >= 0) && (x < NUMBER_OF_DACS) )

#define DAC_BF_WM_SELECTED (1)   //coold be 0, 1, 2 or 3.
#define DAC_BF_WORK_MODE_SELECTED  (0)  //coulde be 0 (normal mode), 1 (swing mode) or 2 (one-time scan mode).

DAC_Type * arrayP2DAC[] = DAC_BASE_PTRS;

DAC_callback_t DAC0callback;

void DACclockGating(void);

int DACgetBFreadPointer(DACids id);

/*Steps to correct initialization:
 * 1) Clock gating.
 * 2) Enable module.
 * 3) Choose Vref for the module.
 * 4) Check configuration and use DAC buffer, triggers and interrupts if necessary.
 */
void DACinit(DACids id, DACconfig_t * config)
{
	DAC_Type * p2DAC;
	if(IS_VALID_ID_DAC(id))
	{
		p2DAC = arrayP2DAC[id];
		//1)
		DACclockGating();
		//2)
		p2DAC->C0 |= DAC_C0_DACEN(1);
		//3)
		p2DAC->C0 &= ~DAC_C0_DACRFS_MASK;
		p2DAC->C0 |= DAC_C0_DACRFS(1);
		//4)
		if(config->mode == DAC_BUFFER_MODE)
		{
			DAC0callback = config->p2callback;

			if(config->dmaMode == DAC_DMA_ENABLE)
			{
				p2DAC->C1 |= DAC_C1_DMAEN(1);
			}
			else
			{
				p2DAC->C1 &= ~DAC_C1_DMAEN_MASK;
			}

			if(config->triggerMode == DAC_TRIGGER_SW)
			{
				p2DAC->C0 |= DAC_C0_DACTRGSEL(1);
			}
			else if(config->triggerMode == DAC_TRIGGER_HW)
			{
				p2DAC->C0 &= ~DAC_C0_DACTRGSEL_MASK;
			}

			p2DAC->C0 |= DAC_C0_DACBWIEN(1) | DAC_C0_DACBTIEN_MASK;   //buffer water mark interrupt enable

			p2DAC->C1 &= ~DAC_C1_DACBFMD_MASK; //selecting the buffer work mode
			p2DAC->C1 |= DAC_C1_DACBFMD(DAC_BF_WORK_MODE_SELECTED);

			p2DAC->C1 &= ~DAC_C1_DACBFWM_MASK;				//selecting the water mark
			p2DAC->C1 |= DAC_C1_DACBFWM(DAC_BF_WM_SELECTED);

			p2DAC->C1 |= DAC_C1_DACBFEN(1);   //enable DAC buffer

			p2DAC->C2 &= ~DAC_C2_DACBFUP_MASK; //selecting DAC buffer size
			p2DAC->C2 |= DAC_C2_DACBFUP(DAC_BUFFER_SIZE - 1);
			NVIC_EnableIRQ(DAC0_IRQn);


		}
		//...
	}
}

void writeDACvalue(DACids id, int bufferIndex, float value)
{
	//assuming that the DACbuffer is not enable, so
	//writing directly to DACdata register...

	//Vo = Vin*(1+DATA[11:0])/4096 --> DATA[11:0] = ((Vo/Vin)*4096)-1
	float Vin = VIN_SELECTED;
	uint16_t DACdataAux = (uint16_t) ((value/Vin)*(float)(4096.0)) - (float)(1.0);
	DAC_Type * p2DAC;
	if(IS_VALID_ID_DAC(id))
	{
		//p2DAC->DAT[0].DATL &= ~DAC_DATL_DATA0_MASK;
		//p2DAC->DAT[0].DATL &= ~DAC_DATL_DATA0_MASK;
		p2DAC = arrayP2DAC[id];
		p2DAC->DAT[bufferIndex].DATL = DAC_DATL_DATA0(DACdataAux);
		p2DAC->DAT[bufferIndex].DATH = DAC_DATH_DATA1(DACdataAux >> 8);
	}
}

void writeDACdata(DACids id, int bufferIndex, int data)
{
	DAC_Type * p2DAC;
	if(IS_VALID_ID_DAC(id))
	{
		//p2DAC->DAT[0].DATL &= ~DAC_DATL_DATA0_MASK;
		//p2DAC->DAT[0].DATL &= ~DAC_DATL_DATA0_MASK;
		p2DAC = arrayP2DAC[id];
		p2DAC->DAT[bufferIndex].DATL = DAC_DATL_DATA0((uint16_t)data);
		p2DAC->DAT[bufferIndex].DATH = DAC_DATH_DATA1(((uint16_t)data) >> 8);
	}
}

int shapeValue2DACdata(float value)
{
	float Vin = VIN_SELECTED;
	int ret = (int)((uint16_t) ((value)*(float)(4096.0)/Vin) - (float)(1.0));
	ret = (ret >= 0)? ret : 0;
	return ret;
}

void DACclockGating(void)
{
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;

	SIM->SCGC2 |= SIM_SCGC2_DAC1_MASK;
}

void updateSoftwareTrigger(DACids id)
{
	DAC_Type * p2DAC;
	if(IS_VALID_ID_DAC(id))
	{
		p2DAC = arrayP2DAC[id];
		p2DAC->C0 |= DAC_C0_DACSWTRG(1);
	}
}

int DACgetBFreadPointer(DACids id)
{
	DAC_Type * p2DAC;
	uint8_t ret = 0;
	if(IS_VALID_ID_DAC(id))
	{
		p2DAC = arrayP2DAC[id];
		ret = ((p2DAC->C2 & DAC_C2_DACBFRP_MASK) >> DAC_C2_DACBFRP_SHIFT);
	}
	return (int) ret;
}
void DACincBFreadPointer(DACids id)
{
	DAC_Type * p2DAC;
	uint8_t aux = 0;
	if(IS_VALID_ID_DAC(id))
	{
		p2DAC = arrayP2DAC[id];
		aux = ((p2DAC->C2 & DAC_C2_DACBFRP_MASK) >> DAC_C2_DACBFRP_SHIFT);  //read and save the previous.
		p2DAC->C2 &= ~DAC_C2_DACBFRP_MASK;  //clear read pointer.
		p2DAC->C2 |= DAC_C2_DACBFRP(aux+1);  //write new value (previous value incremented by one).
	}
}

void DAC0_IRQHandler(void)
{
	if(DAC0->SR & DAC_SR_DACBFWMF_MASK)
	{
		DAC0callback(DAC_WM_RP_EV);
		DAC0->SR &= ~DAC_SR_DACBFWMF_MASK;
	}
	else if(DAC0->SR & DAC_SR_DACBFRPTF_MASK)
	{
		DAC0callback(DAC_RESET_RP_EV);
		DAC0->SR &= ~DAC_SR_DACBFRPTF_MASK;
	}

}
