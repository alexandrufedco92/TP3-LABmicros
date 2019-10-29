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

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
    uint32_t source_address;					//Source address
    uint32_t dest_address;                  //destination address
    uint8_t source_transf_size;  			//source transfer size
    uint8_t dest_transf_size; 				//destination transfer size
    int16_t source_offset;
    int16_t dest_offset;
    uint32_t minor_loop_bytes;				//bytes a transferir en cada minor loop
    uint32_t major_loop_count;              //contador de major loops
}dma_transfer_conf_t;

enum dma_modes{MEM_2_MEM, MEM_2_PERIPHERAL, PERIPHERAL_2_MEM};
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

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
void DMAPrepareTransfer(uint8_t id, dma_transfer_conf_t* config, uint8_t mode);

/**
 * @brief
 * @param
 * @return
*/
bool isDMAnTransferDone(uint8_t id);

#endif /* DMA_H_ */
