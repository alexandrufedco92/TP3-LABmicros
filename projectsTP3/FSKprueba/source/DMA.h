/*
 * DMA.h
 *
 *  Created on: Oct 21, 2019
 *      Author: Lu
 */

#ifndef DMA_H_
#define DMA_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DMA_WAVEGEN_CH 	1
#define DMA_PIT1		59
#define DMA_FTM1_CH0 	28
#define DMA_CAPTURE_CH 0

#define DMA_FTM2_CH0 30

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum dma_bytes_to_transfer{BITS_8 = 0x00, BITS_16 = 0x01, BITS_32 = 0x02};
typedef enum{MEM_2_MEM, MEM_2_PERIPHERAL, PERIPHERAL_2_MEM}dma_mode_t;
typedef void (*dmaFun_t)(void);


typedef struct{
    uint32_t source_address;					//Source address
    uint32_t dest_address;                  //destination address
    uint8_t transf_size;  			//transfer size
    int16_t offset;
    uint32_t bytes_per_request;				//bytes a transferir en cada minor loop
    uint32_t total_bytes;              //cantidad total de bytes a transferir
    dma_mode_t mode;

    uint8_t channel;			//info de dma_mux
    uint16_t request_source;	//request source for dma
    bool periodic_trigger;		//enable periodic trigger
    dmaFun_t dma_callback;		//callback para cuando termina major loop
}dma_transfer_conf_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/**
 * @brief initializes DMA module
*/
void initDMA(void);

//carga los registros para una transferencia sin channel linking
void DMAPrepareTransferELINKNO(dma_transfer_conf_t* config);

//carga los registros para una transferencia CON channel linking
void DMAPrepareTransferELINKYES(dma_transfer_conf_t* config);


#endif /* DMA_H_ */
