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
#include "DMAMUX.h"
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

uint16_t sourceBuffer[10] = {0x1234,0x6789,0x1122,0x2233,0x5588,0x2345,0x3145,0x8172,0x6183,0x3756};
uint8_t destinationBuffer[10];
dma_transfer_conf_t conf;

void dummy(void){
	int dummy = 0;
	dummy++;
}
#define DMA_EXAMPLE 2



void App_Init (void)
{
	initCMP(CMP_0);
	initDMA();

	config_t pit_conf;
	pit_conf.chainMode[DMA_EXAMPLE] = false;
	pit_conf.interruptEnable[DMA_EXAMPLE] = true;
	pit_conf.pitCallbacks[DMA_EXAMPLE] = &dummy;
	pit_conf.timerVal[DMA_EXAMPLE] = 1000;
	pit_conf.timerEnable[DMA_EXAMPLE] = true;

	configureDMAMUX(DMA_EXAMPLE, 60, true);
	conf.source_address = (uint32_t)sourceBuffer;
	conf.dest_address = (uint32_t)destinationBuffer;
	conf.source_offset = 0x02;
	conf.dest_offset = 0x01;
	conf.source_transf_size = 0; //1 byte
	conf.dest_transf_size = 0; //1 byte
	conf.minor_loop_bytes = 0x05;
	conf.major_loop_count = 0x01;

	DMAPrepareTransfer(DMA_EXAMPLE, &conf);
	PITinit(&pit_conf);
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
