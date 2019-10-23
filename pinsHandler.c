/*
 * pinsHandler.c
 *
 *  Created on: 18 sep. 2019
 *      Author: G5
 */

#include "pinsHandler.h"

// Digital values
#ifndef LOW
#define LOW     0
#define HIGH    1
#endif // LOW



_Bool isPinValid(pin_t pin)
{
	_Bool ret = false;
	if((pin >= PORTNUM2PIN(PA,0)) && (pin <= PORTNUM2PIN(PE,31)))
	{
		ret = true;
	}

	return ret;
}

void setPCRmux(PORT_Type * p2port, uint8_t numPin, uint8_t mux)
{
	uint32_t actualPCR = (p2port->PCR)[numPin]; //obtengo el antiguo PCR para luego actualizarlo
	actualPCR = (actualPCR) & (~PORT_PCR_MUX_MASK); //elimino el contenido anterior del mux del PCR
	actualPCR = actualPCR | PORT_PCR_MUX(mux); //ingreso el mux con el contenido deseado por el usuario de esta función
	(p2port->PCR)[numPin] = actualPCR; //finalmente actualizo el PCR

}

void setPCRpullEnable(PORT_Type * portPointer, uint8_t numPin)
{
	uint32_t maskPE = (HIGH << PORT_PCR_PE_SHIFT);
	(portPointer->PCR)[numPin] = ((portPointer->PCR)[numPin] | maskPE);
}
void setPCRpullUp(PORT_Type * portPointer, uint8_t numPin)
{
	uint32_t maskPE = (HIGH << PORT_PCR_PS_SHIFT);
	(portPointer->PCR)[numPin] = ((portPointer->PCR)[numPin] | maskPE);
}
void setPCRpullDown(PORT_Type * portPointer, uint8_t numPin)
{
	uint32_t maskPE = (HIGH << PORT_PCR_PE_SHIFT);
	(portPointer->PCR)[numPin] = ((portPointer->PCR)[numPin] & (~maskPE));
}


void setPCRopenDrainEnable(PORT_Type * portPointer, uint8_t numPin)
{
	(portPointer->PCR)[numPin] = ((portPointer->PCR)[numPin] | PORT_PCR_ODE_MASK);
}



void setPCRirqc(PORT_Type * p2port, uint8_t numPin, uint8_t irqMode)
{
	uint32_t actualPCR = (p2port->PCR)[numPin]; //obtengo el antiguo PCR para luego actualizarlo
	actualPCR = actualPCR & (~PORT_PCR_IRQC_MASK);
	actualPCR = actualPCR | PORT_PCR_IRQC(irqMode);
	(p2port->PCR)[numPin] = actualPCR; //finalmente actualizo el PCR
}



