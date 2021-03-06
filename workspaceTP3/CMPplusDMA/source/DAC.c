/*
 * DAC.c
 *
 *  Created on: 17 oct. 2019
 *      Author: G5
 */

#include "DAC.h"
#include "pinsHandler.h"



#define IS_VALID_ID_DAC(x) ( (x >= 0) && (x < NUMBER_OF_DACS) )

DAC_Type * arrayP2DAC[] = DAC_BASE_PTRS;

void DACclockGating(void);

/*Steps to correct initialization:
 * 1) Clock gating.
 * 2) Enable module.
 * 3) Choose Vref for the module.
 * 4) Check configu and use DAC buffer, triggers and interrupts if necessary.
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
		//...
	}
}

void writeDACvalue(DACids id, float value)
{
	//assuming that the DACbuffer is not enable, so
	//writing directly to DACdata register...

	//Vo = Vin*(1+DATA[11:0])/4096 --> DATA[11:0] = ((Vo/Vin)*4096)-1
	float Vin = 3.3;
	uint16_t DACdataAux = (uint16_t) ((value/Vin)*(float)(4096.0)) - (float)(1.0);
	DAC_Type * p2DAC = arrayP2DAC[id];
	if(IS_VALID_ID_DAC(id))
	{
		//p2DAC->DAT[0].DATL &= ~DAC_DATL_DATA0_MASK;
		//p2DAC->DAT[0].DATL &= ~DAC_DATL_DATA0_MASK;
		p2DAC->DAT[0].DATL = DAC_DATL_DATA0(DACdataAux);
		p2DAC->DAT[0].DATH = DAC_DATH_DATA1(DACdataAux >> 8);
	}
}

void DACclockGating(void)
{
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
	//SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;

	SIM->SCGC2 |= SIM_SCGC2_DAC1_MASK;
}
