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

void App_Init (void)
{
	initCMP(CMP_0);
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
