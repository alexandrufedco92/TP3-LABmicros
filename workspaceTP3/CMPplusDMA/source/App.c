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

void App_Init (void)
{
	initCMP(CMP_0);
	initDMA();
	configureDMAMUX(0, 51, false);
	dma_transfer_conf_t conf;
	conf.source_address = (uint32_t)sourceBuffer;
	conf.dest_address = (uint32_t)destinationBuffer;
	conf.source_offset = 0x02;
	conf.dest_offset = 0x01;
	conf.source_transf_size = 0; //1 byte
	conf.dest_transf_size = 0; //1 byte
	conf.minor_loop_bytes = 0x01;
	conf.major_loop_count = 0x05;

	DMAPrepareTransfer(0, &conf);
	isDMAnTransferDone(0);
//	DACconfig_t DACconfig;
//	initResourcesController2pc();
//	InitializeTimers();
//	DACinit(DAC0, &DACconfig);
//	writeDACvalue(DAC0, 2.0);
//	SetTimer(SENOIDAL, 1, senoidalCallback);
}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
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

void senoidalCallback(void)
{
	static float t = 0.0;

	if(t >= 0.010)
	{
		t = 0.000;
	}
	else
	{
		t += 0.001;
	}
	float valueAux = 1.0000 + sin(2*M_PI*t*100); //100Hz
	writeDACvalue(DAC0, valueAux);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
