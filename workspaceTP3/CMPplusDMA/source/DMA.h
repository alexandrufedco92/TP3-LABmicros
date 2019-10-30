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
#include "DMAMUX.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum dma_bytes_to_transfer{BITS_8 = 0x00, BITS_16 = 0x01, BITS_32 = 0x02};
typedef enum{MEM_2_MEM, MEM_2_PERIPHERAL, PERIPHERAL_2_MEM}dma_mode_t;

typedef struct{
    uint32_t source_address;					//Source address
    uint32_t dest_address;                  //destination address
    uint8_t transf_size;  			//transfer size
    int16_t offset;
    uint32_t bytes_per_request;				//bytes a transferir en cada minor loop
    uint32_t total_bytes;              //cantidad total de bytes a transferir
    dma_mode_t mode;
}dma_transfer_conf_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/**
 * @brief initializes DMA module
*/
void initDMA(void);

/**
 * @brief
 * @param
 * @return
*/
void DMAPrepareTransfer(uint8_t id, dma_transfer_conf_t* config);

/**
 * @brief
 * @param
 * @return
*/
bool isDMAnTransferDone(uint8_t id);

#endif /* DMA_H_ */
