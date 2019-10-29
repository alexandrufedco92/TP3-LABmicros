/*
 * DMA.c
 *
 *  Created on: Oct 21, 2019
 *      Author: Lu
 */

#include "MK64F12.h"
#include "DMA.h"
#include "DMAMUX.h"

//DMA_Type* dma_ptrs[] = DMA_BASE_PTRS;
//IRQn_Type arrayDMAirqs[] = DMA_CHN_IRQS;

bool finished[DMA_CHANNEL_COUNT];




void clockGating(){
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
}

void initDMA(void){
	/**
	 * steps for correct initialization:
	 *
	 * 1. clock gating de perifericos dma & dmaMux
	 * 2. llamar a configureDMAMUX(dma_mux_channels channel, dma_request_source_t source, bool periodic_trigger)
	 * 		solo los primeros 4 channels tienen periodic trigger
	 * 3. set enable request register
	 * 4. fill the tcd structure
	 * 		a. set source and destination address
	 * 		b. set source and destination address offset
	 * 		c. set data size to be read and written
	 * 		d. number of bytes to be transferred in each service request of the channel must be specified
	 * 		e. current major iteration count (CITER) and beginning iteration count (BITER) initialized to same value (elink=no)
	 * 		f. source- and destination-address adjustment must be specified
	 * 		g. the Control and Status register (DMA_TCDn_CSR) must be setup
	 *
	*/
	clockGating();
	initDMAMUX();	//clock gating dmamux
	NVIC_EnableIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA1_IRQn);
	NVIC_EnableIRQ(DMA2_IRQn);
	NVIC_EnableIRQ(DMA3_IRQn);
//	NVIC_EnableIRQ(DMA_Error_IRQn);
}

bool isDMAnTransferDone(uint8_t id){
	return finished[id];
}


void DMAPrepareTransfer(uint8_t id, dma_transfer_conf_t* config, uint8_t mode){
	finished[id] = false;
	DMA0->TCD[id].SADDR = config->source_address;
	DMA0->TCD[id].DADDR = config->dest_address;

	DMA0->TCD[id].SOFF = config->source_offset;
	DMA0->TCD[id].DOFF = config->dest_offset;

	DMA0->TCD[id].ATTR = DMA_ATTR_SSIZE(config->source_transf_size) | DMA_ATTR_DSIZE(config->dest_transf_size);

	uint32_t nbytes = config->minor_loop_bytes;
	DMA0->TCD[id].NBYTES_MLNO = nbytes;

	uint32_t citer = config->major_loop_count;
	DMA0->TCD[id].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(config->major_loop_count);
	DMA0->TCD[id].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(config->major_loop_count);

	if(mode == PERIPHERAL_2_MEM)
	{
		DMA0->TCD[id].SLAST = 0x00;
		DMA0->TCD[id].DLAST_SGA = -citer*nbytes;		//memory to peripheral
	}
	if(mode == MEM_2_PERIPHERAL)
	{
		DMA0->TCD[id].SLAST = -citer*nbytes;
		DMA0->TCD[id].DLAST_SGA = 0x00;		//memory to peripheral
	}
	else		//memory to memory
	{
		//todo
	}


	DMA0->TCD[id].CSR = DMA_CSR_INTMAJOR_MASK;		//enable major interrupt

	switch (id)
	{
	case 0:
		DMA0->ERQ = DMA_ERQ_ERQ0_MASK;
		break;
	case 1:
		DMA0->ERQ = DMA_ERQ_ERQ1_MASK;
		break;
	case 2:
		DMA0->ERQ = DMA_ERQ_ERQ2_MASK;
		break;
	case 3:
		DMA0->ERQ = DMA_ERQ_ERQ3_MASK;
		break;
	}

}

void DMAIrqHandler(void){
	DMA0->CINT |= 0;
}

void DMA0_IRQHandler(){
	DMAIrqHandler();
	finished[0] = true;
}

void DMA1_IRQHandler(){
	DMAIrqHandler();
	finished[1] = true;
}

void DMA2_IRQHandler(){
	DMAIrqHandler();
	finished[2] = true;
}

void DMA3_IRQHandler(){
	DMAIrqHandler();
	finished[3] = true;
}



