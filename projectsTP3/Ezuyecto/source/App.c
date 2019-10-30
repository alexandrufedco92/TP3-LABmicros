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
#include <math.h>
#include <stdint.h>
#include "gpio.h"
#include "FSK_Demodulator.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MARK_KEY '1'
#define SPACE_KEY '0'

#define IS_MARK_FREQ(f) ( (f >= 1000) && (f <= 1400) )
#define IS_SPACE_FREQ(f) ( (f >= 2200) && (f <= 2600) )
#define SIZE 80
#define PI 3.1415926535
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static float vector[SIZE];

void App_Init (void)
{
	DemodulatorInit();
	gpioMode( PORTNUM2PIN(PB,18), OUTPUT);
	int i =0;
	float sample_freq = 12000;
	int f = 2200;

	for(i=0; i<20; i++)
	{
		vector[i] = sin( (2*PI*f*i)/sample_freq );
	}
	f=1200;
	for(i=20; i<40; i++)
	{
		vector[i] = sin( (2*PI*f*i)/sample_freq );
	}
	f=2200;
	for(i=40; i<50; i++)
	{
		vector[i] = sin( (2*PI*f*i)/sample_freq );
	}
	f=1200;
	for(i=50; i<60; i++)
	{
		vector[i] = sin( (2*PI*f*i)/sample_freq );
	}
	f=2200;
	for(i=60; i<SIZE; i++)
	{
		vector[i] = sin( (2*PI*f*i)/sample_freq );
	}

}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	int8_t aux =0;
	int j=0;
	int i =0;
	int8_t signal_ezu[30];
	for(i=0; i<SIZE;i++)
	{
		gpioWrite (PORTNUM2PIN(PB,18), true);
		aux = DemodulateSignal(vector[i]);
		gpioWrite ( PORTNUM2PIN(PB,18), false );
		if( aux != -1)
		{
			signal_ezu[j] = aux;
			j++;
		}
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
