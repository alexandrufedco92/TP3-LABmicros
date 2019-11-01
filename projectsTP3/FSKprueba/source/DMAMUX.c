/*
 * DMAMUX.c
 *
 *  Created on: Oct 26, 2019
 *      Author: Lu
 */

#include "MK64F12.h"
#include "DMAMUX.h"




static void clockGating();



void clockGating(){
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}

void initDMAMUX(){
	clockGating();
}

void configureDMAMUX(dma_mux_channels channel, uint16_t source, bool periodic_trigger){

	DMAMUX->CHCFG[channel] &= ~DMAMUX_CHCFG_SOURCE_MASK;	//borro la source actual

	DMAMUX->CHCFG[channel] |= DMAMUX_CHCFG_SOURCE(source);	//source que quiero

	if(periodic_trigger)
		if(channel < 4)
			DMAMUX->CHCFG[channel] |= DMAMUX_CHCFG_TRIG_MASK;

//	dma_mux_ptrs->CHCFG[channel] |= DMAMUX_CHCFG_A_ON_MASK;	//todo qué onda ésto

	DMAMUX->CHCFG[channel] |= DMAMUX_CHCFG_ENBL_MASK;	//enable channel
}






