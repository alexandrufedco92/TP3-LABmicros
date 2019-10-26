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
#define SIZE 20

#define __FOREVER__ 	for(;;)



int main (void)
{
	/*
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
	 */




	hw_DisableInterrupts();
	DACconfig_t config;
	config.mode = DAC_NORMAL_MODE;
	config.dmaMode = DAC_DMA_DISABLE;
	DACinit(DAC0_ID, &config);
	//ADC Init
	ADC_Config_t adc_config;
	adc_config.channel_sel = AD5;
	adc_config.clock_type = BUS_CLOCK;
	adc_config.clock_divide = DIVIDE_BY_2;
	adc_config.low_power = true;
	adc_config.diffential_mode = false;
	adc_config.enable_interrupts = false;
	adc_config.voltage_reference = DEFAULT;
	adc_config.trigger = SOFTWARE_TRIGGER;
	adc_config.resolution = SIXTEEN_BITS;
	adc_config.id = FIRST_ADC;
	adc_config.enable_hardware_avg = false;
	ADC_Init( &adc_config );

	float value = VIN_SELECTED/2.0;
	writeDACvalue(DAC0_ID, 0, value);
	// Enable interrupts
	hw_EnableInterrupts();

	int16_t adc_data = 0;

	while(1)
	{
		if( IsConversionFinished() )
		{
			adc_data = GetConversionResult();
		}
	}


}

