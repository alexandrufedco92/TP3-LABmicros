/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
/*

*/
#include "comController2pc.h"
#include "DAC.h"
#include "CMP.h"
#include "DMA.h"
#include "PIT.h"
#include "timer.h"
#include <math.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void senoidalCallback(void);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */

uint16_t sourceBuffer[16] = {0x1234,0x6789,0x1122,0x2233,0x5588,0x2345,0x3145,0x8172,0x6183,0x3756};
uint16_t destiny;

uint16_t destinationBuffer[10] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
uint16_t origin = 0x0001;

dma_transfer_conf_t conf;

void dummy(void){
	int dummy;
}
#define DMA_WAVEGEN_CH 0
#define DMA_FTM1_CH0 28

void App_Init (void)
{
	initCMP(CMP_0);
	initDMA();

//	config_t pit_conf;
//	pit_conf.chainMode[DMA_WAVEGEN_CH] = false;
//	pit_conf.interruptEnable[DMA_EXAMPLE] = true;
//	pit_conf.pitCallbacks[DMA_EXAMPLE] = &dummy;
//	pit_conf.timerVal[DMA_EXAMPLE] = 1000;
//	pit_conf.timerEnable[DMA_EXAMPLE] = true;

//	//memory to peripheral ejemplo!!
	configureDMAMUX(DMA_WAVEGEN_CH, DMA_FTM1_CH0, false);
	conf.source_address = (uint32_t)sourceBuffer;
	conf.dest_address = (uint32_t)&destiny;
	conf.offset = 0x02;
	conf.transf_size = BITS_16;
	conf.bytes_per_request = 0x02;
	conf.total_bytes = 0x20;
	conf.mode = MEM_2_PERIPHERAL;
	DMAPrepareTransfer(DMA_WAVEGEN_CH, &conf);

	//peripheral to memory ejemplo!!
//	configureDMAMUX(DMA_EXAMPLE, 60, true);
//	conf.source_address = (uint32_t)&origin;
//	conf.dest_address = (uint32_t)destinationBuffer;
//	conf.offset = 0x02;
//	conf.transf_size = BITS_16;
//	conf.bytes_per_request = 0x02;
//	conf.total_bytes = 0x04;
//	conf.mode = PERIPHERAL_2_MEM;
//	DMAPrepareTransfer(DMA_EXAMPLE, &conf);

//	PITinit(&pit_conf);
}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	bool dummy1 = false;
	if(isDMAnTransferDone(2))
		 dummy1 = true;
	/* Main program to generate a sinusoidal signal with de DAC, controlling the frequency
	 * by an input voltage to the ADC:
	 * The program waits a timer's interrupt that increments a global float variable t and
	 * put to the DAC the nearest value of sin(2*PI*t/T), while the variable T is being
	 * changed by the input of the ADC. It's important to decrement the variable t doing t - T
	 * only when t is higher than T.
	 */

	/*Main program to probe FTM (TIMER, OUTPUT COMPARE, INPUT CAPTURE and PWM):
	 *
	 */
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
