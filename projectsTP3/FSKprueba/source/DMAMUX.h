/*
 * DMAMUX.h
 *
 *  Created on: Oct 26, 2019
 *      Author: Lu
 */

#ifndef DMAMUX_H_
#define DMAMUX_H_

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

typedef enum {DMA_CHANNEL_0, DMA_CHANNEL_1, DMA_CHANNEL_2, DMA_CHANNEL_3,DMA_CHANNEL_COUNT}dma_mux_channels;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/**
 * @brief initializes DMAMUX module
*/
void initDMAMUX();

//configura el canal, el request source para el dma request, y si se usará un PIT timer en periodic trigger (true si se usa)
void configureDMAMUX(dma_mux_channels channel, uint16_t source, bool periodic_trigger);


#endif /* DMAMUX_H_ */
