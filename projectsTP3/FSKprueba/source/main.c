/*
 * main.c
 *
 *  Created on: May 1, 2015
 *      Author: Juan Pablo VEGA - Laboratorio de Microprocesadores
 */

#include "hardware.h"
#include <math.h>
#include <stdint.h>
#include "FSK_Demodulator.h"

#define PI 3.14159
#define SIZE 20

#define __FOREVER__ 	for(;;)



int main (void)
{

	int16_t vector[SIZE];
	int i =0;
	uint32_t sample_freq = 12000;
	int f = 1200;
	DemodulatorInit(sample_freq);
	float amplitude = 100;
	for(i=0; i<SIZE; i++)
	{
		vector[i] = (int16_t) ( amplitude*sin( (2*PI*i*f)/sample_freq ) );
	}

	DemodulateSignal(vector, SIZE);





	// 		hw_DisableInterrupts();

	__FOREVER__;

	// Enable interrupts
	//hw_EnableInterrupts();

}

