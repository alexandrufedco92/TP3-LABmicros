/*
 * DMA.c
 *
 *  Created on: Oct 21, 2019
 *      Author: Lu
 */

#include "MK64F12.h"
#include "DMA.h"
#include "DMAMUX.h"


#define NULL 0
static dmaFun_t dmaFuns[4];
static bool dma_initialized = false;


void clockGatingDMA(){
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
}

void initDMA(void){
	/**
	 * steps for correct initialization:
	 *
	 * 1. clock gating de perifericos dma & dmaMux
	 * 2. llamar a confureDMAMUX(dma_mux_channels channel, dma_request_source_t source, bool periodic_trigger)
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
	if(!dma_initialized)
	{
		clockGatingDMA();
		initDMAMUX();	//clock gating dmamux
		dma_initialized = true;
		//no habilitamos nvic, no usamos interrupciones
	}
}


void DMAPrepareTransferELINKNO(dma_transfer_conf_t* conf){

	uint8_t id = conf->channel;
	dmaFuns[id] = conf->dma_callback;
	configureDMAMUX(conf->channel, conf->request_source, conf->periodic_trigger);
	DMA0->TCD[id].SADDR = conf->source_address;
	DMA0->TCD[id].DADDR = conf->dest_address;

	DMA0->TCD[id].ATTR = DMA_ATTR_SSIZE(conf->transf_size) | DMA_ATTR_DSIZE(conf->transf_size);

	uint32_t nbytes = conf->bytes_per_request;
	DMA0->TCD[id].NBYTES_MLNO = nbytes;

	uint32_t citer = conf->total_bytes/conf->bytes_per_request;
	DMA0->TCD[id].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(citer);
	DMA0->TCD[id].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(citer);

	if(conf->mode == PERIPHERAL_2_MEM)		//peripheral to memory
	{
		DMA0->TCD[id].SOFF = 0x00;		//siempre del mismo lugar
		DMA0->TCD[id].DOFF = conf->offset;
		DMA0->TCD[id].SLAST = 0x00;		//siempre del mismo lugar
		DMA0->TCD[id].DLAST_SGA = -citer*nbytes;
	}
	else if(conf->mode == MEM_2_PERIPHERAL)		//memory to peripheral
	{
		DMA0->TCD[id].SLAST = -citer*nbytes;
		DMA0->TCD[id].DLAST_SGA = 0x00;		//siempre al mismo lugar
		DMA0->TCD[id].SOFF = conf->offset;
		DMA0->TCD[id].DOFF = 0x00;			//siempre al mismo lugar
	}
	else		//memory to memory
	{
		DMA0->TCD[id].SLAST = -citer*nbytes;		//actualizo direcciones
		DMA0->TCD[id].DLAST_SGA = -citer*nbytes;	//actualizo direcciones
		DMA0->TCD[id].SOFF = conf->offset;
		DMA0->TCD[id].DOFF = conf->offset;
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

void DMAPrepareTransferELINKYES(dma_transfer_conf_t* conf){

	uint8_t id = conf->channel;
	dmaFuns[id] = conf->dma_callback;
	configureDMAMUX(conf->channel, conf->request_source, conf->periodic_trigger);
	DMA0->TCD[id].SADDR = (uint32_t)(conf->source_address);
	DMA0->TCD[id].DADDR = (uint32_t)(conf->dest_address);

	DMA0->TCD[id].ATTR = DMA_ATTR_SSIZE(conf->transf_size) | DMA_ATTR_DSIZE(conf->transf_size);

	uint32_t nbytes = conf->bytes_per_request;
	DMA0->TCD[id].NBYTES_MLNO = nbytes;

	uint32_t citer = conf->total_bytes/conf->bytes_per_request;
	DMA0->TCD[id].CITER_ELINKYES |= DMA_CITER_ELINKYES_ELINK_MASK;	//enable elink
	DMA0->TCD[id].CITER_ELINKYES |= DMA_CITER_ELINKYES_LINKCH(id-1);		//configuro channel linking para que el canal sea trigereado por el anterior
	DMA0->TCD[id].CITER_ELINKYES |= DMA_CITER_ELINKYES_CITER(citer);		//es decir, cuando termina una trasnfer del channel n, se triggerea el n+1
	DMA0->TCD[id].BITER_ELINKYES |= DMA_BITER_ELINKYES_ELINK_MASK;	//enable elink
	DMA0->TCD[id].BITER_ELINKYES |= DMA_BITER_ELINKYES_LINKCH(id-1);
	DMA0->TCD[id].BITER_ELINKYES |= DMA_BITER_ELINKYES_BITER(citer);

	DMA0->TCD[id].CSR |= DMA_CSR_MAJORLINKCH(id-1);
	DMA0->TCD[id].CSR |= DMA_CSR_MAJORELINK_MASK;

	if(conf->mode == PERIPHERAL_2_MEM)		//peripheral to memory
	{
		DMA0->TCD[id].SOFF = 0x00;		//del mismo lugar
		DMA0->TCD[id].DOFF = conf->offset;
		DMA0->TCD[id].SLAST = 0x00;
		DMA0->TCD[id].DLAST_SGA = -citer*nbytes;
	}
	else if(conf->mode == MEM_2_PERIPHERAL)		//memory to peripheral
	{
		DMA0->TCD[id].SLAST = -citer*nbytes;
		DMA0->TCD[id].DLAST_SGA = 0x00;		//al mismo lugar
		DMA0->TCD[id].SOFF = conf->offset;
		DMA0->TCD[id].DOFF = 0x00;				
	}
	else		//memory to memory
	{
		DMA0->TCD[id].SLAST = -citer*nbytes;
		DMA0->TCD[id].DLAST_SGA = -citer*nbytes;
		DMA0->TCD[id].SOFF = conf->offset;
		DMA0->TCD[id].DOFF = conf->offset;
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


void DMA0_IRQHandler(){		//no habilitamos interrupciones asi que nunca entra a esta funcion
	DMA0->CINT |= 0;
	if(dmaFuns[0] != NULL)
		dmaFuns[0]();
}

void DMA1_IRQHandler(){		//no habilitamos interrupciones asi que nunca entra a esta funcion
	DMA0->CINT |= 0;
	if(dmaFuns[1] != NULL)
		dmaFuns[1]();
}

void DMA2_IRQHandler(){		//no habilitamos interrupciones asi que nunca entra a esta funcion
	DMA0->CINT |= 0;
	if(dmaFuns[2] != NULL)
			dmaFuns[2]();
}

void DMA3_IRQHandler(){		//no habilitamos interrupciones asi que nunca entra a esta funcion
	DMA0->CINT |= 0;
	if(dmaFuns[3] != NULL)
		dmaFuns[3]();
}



