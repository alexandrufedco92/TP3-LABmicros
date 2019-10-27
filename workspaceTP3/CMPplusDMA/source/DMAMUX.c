/*
 * DMAMUX.c
 *
 *  Created on: Oct 26, 2019
 *      Author: Lu
 */

#include "MK64F12.h"
#include "DMAMUX.h"

DMAMUX_Type * dma_mux_ptrs[] = DMAMUX_BASE_PTRS;




static void clockGating();



void clockGating(){
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

}

void initDMAMUX(dma_mux_channels channel, dma_mux_sources source, bool periodic_trigger){
	clockGating();

	dma_mux_ptrs->CHCFG[channel] &= ~DMAMUX_CHCFG_SOURCE_MASK;	//borro la source actual

	dma_mux_ptrs->CHCFG[channel] |= DMAMUX_CHCFG_SOURCE(source);	//source que quiero

	if(periodic_trigger)
		dma_mux_ptrs->CHCFG[channel] |= DMAMUX_CHCFG_TRIG_MASK;

//	dma_mux_ptrs->CHCFG[channel] |= DMAMUX_CHCFG_A_ON_MASK;	//todo qué onda ésto

	dma_mux_ptrs->CHCFG[channel] |= DMAMUX_CHCFG_ENBL_MASK;	//enable channel
}






