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

typedef enum {DMA_CHANNEL_0, DMA_CHANNEL_1, DMA_CHANNEL_2, DMA_CHANNEL_3, DMA_CHANNEL_4, DMA_CHANNEL_5, DMA_CHANNEL_6, DMA_CHANNEL_7,
	DMA_CHANNEL_8, DMA_CHANNEL_9, DMA_CHANNEL_10, DMA_CHANNEL_11, DMA_CHANNEL_12, DMA_CHANNEL_13, DMA_CHANNEL_14, DMA_CHANNEL_15,DMA_CHANNEL_COUNT}dma_mux_channels;


//typedef enum{ FTM0_CH0=20, FTM0_CH1=21, FTM0_CH2=22, FTM0_CH3=23, FTM0_CH4=24, FTM0_CH5=25, FTM0_CH6=26, FTM0_CH7=27,
//			FTM1_CH0=28, FTM1_CH1=29, FTM2_CH0=30, FTM2_CH1=31,
//			FTM3_CH0=32, FTM3_CH1=33, FTM3_CH2=34, FTM3_CH3=35, FTM3_CH4=36, FTM3_CH5=37, FTM3_CH6=38, FTM3_CH7=39}dma_mux_sources;

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

void configureDMAMUX(dma_mux_channels channel, dma_request_source_t source, bool periodic_trigger);


#endif /* DMAMUX_H_ */
