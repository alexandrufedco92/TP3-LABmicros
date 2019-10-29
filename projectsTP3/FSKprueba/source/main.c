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
#define SIZE 80

#define __FOREVER__ 	for(;;)

void adc_func(void);


static ADC_Data_t adc_data;

int main (void)
{
	bool signal[30];
	int aux;
	float vector[SIZE];
	int i =0;
	int j = 0;
	float sample_freq = 12000;
	int f = 2200;
	DemodulatorInit();
	float amplitude = 100;
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
	for(i=0; i<SIZE;i++)
	{
		aux = DemodulateSignal(vector[i]);
		if( aux != -1)
		{
			signal[j] = aux;
			j++;
		}
	}

	while(1);


}


