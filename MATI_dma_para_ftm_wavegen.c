#include "DMA.h"

#define DMA_WAVEGEN_CH 0    //esto va a ir en el DMA.h
#define DMA_FTM1_CH0 28     //esto va a ir en el DMA.h


//memory to peripheral ejemplo
dma_transfer_conf_t conf;

configureDMAMUX(DMA_WAVEGEN_CH, DMA_FTM1_CH0, false);
conf.source_address = (uint32_t)arreglo_con_valores;
conf.dest_address = (uint32_t)&registro;
conf.offset = 0x02;
conf.transf_size = BITS_16;
conf.bytes_per_request = 0x02;	//paso 16bits=2bytes en cada dma request
conf.total_bytes = bytes_per_request*16;	//el total será 2bytes*16
conf.mode = MEM_2_PERIPHERAL;
DMAPrepareTransfer(DMA_WAVEGEN_CH, &conf);