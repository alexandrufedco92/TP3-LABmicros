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
#include "DAC.h"
#include "ADC.h"

#define PI 3.14159
#define SIZE 45

#define __FOREVER__ 	for(;;)

void adc_func(void);


static ADC_Data_t adc_data;

int main (void)
{

	float vector[SIZE];
	int i =0;
	float sample_freq = 12000;
	int f = 2200;
	DemodulatorInit(sample_freq);
	float amplitude = 100;
	for(i=0; i<SIZE; i++)
	{
		vector[i] = sin( (2*PI*f*i)/sample_freq );
	}

	DemodulateSignal(vector, SIZE);



}


