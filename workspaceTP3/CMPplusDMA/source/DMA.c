/*
 * DMA.c
 *
 *  Created on: Oct 21, 2019
 *      Author: Lu
 */

#include "MK64F12.h"


static void DMAClockGating();



void DMAClockGating(){
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

}
