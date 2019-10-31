#include "DMA.h"

#define DMA_EXAMPLE 2    //esto va a ir en el DMA.h
#define DMA_FTMX_CHX 60     //esto va a ir en el DMA.h, hay que cambiarlo por el ftm que se va a usar



//peripheral to memory ejemplo
dma_transfer_conf_t conf;

configureDMAMUX(DMA_EXAMPLE, DMA_FTMX_CHX, false);
conf.source_address = (uint32_t)&registro;      //al registro 
conf.dest_address = (uint32_t)arreglo_de_2;     //al arreglo que tiene para poner los dos valores
conf.offset = 0x02;         //paso dos bytes (=16 bits)
conf.transf_size = BITS_16;
conf.bytes_per_request = 0x02;  //2 bytes
conf.total_bytes = 0x04;            //el total es 2bytes*2
conf.mode = PERIPHERAL_2_MEM;
DMAPrepareTransfer(DMA_EXAMPLE, &conf);
