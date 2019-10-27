z/*
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


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
    uint32_t source_address;                      /*!< Source data address. */
    uint32_t dest_address;                     /*!< Destination data address. */
    uint8_t source_transf_size;  			/*!< Source data transfer size. */
    uint8_t dest_transf_size; 				/*!< Destination data transfer size. */
    int16_t source_offset;                     /*!< Sign-extended offset applied to the current source address to
                                                form the next-state value as each source read is completed. */
    int16_t dest_offset;                    /*!< Sign-extended offset applied to the current destination address to
                                                form the next-state value as each destination write is completed. */
    uint32_t minor_loop_bytes;               /*!< Bytes to transfer in a minor loop*/
    uint32_t major_loop_count;              /*!< Major loop iteration count. */
}dma_transfer_conf_t;
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
 * @param
 * @param
 * @return
*/
void setDMA(uint8_t channel);


#endif /* DMA_H_ */
