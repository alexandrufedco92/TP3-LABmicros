/*
 * FSK_Demodulator.c
 *
 *  Created on: Oct 19, 2019
 *      Author: G5
 */

#include "FSK_Demodulator.h"
#include <stdbool.h>
/***********************************************************
 *					 DEFINES AND MACROS
 ***********************************************************/
#define FL 1200	//Frequency of a logical '1'
#define FH 2200	//Frequency of a logical '0'
#define DELAY 0.0004464 //Delay optimo en s
#define VLOW -0.25
#define VHIGH 0.25
#define FIR_ORDER 18
//Size of Buffers
#define CIRCULAR_BUFFER_SIZE 80

typedef struct{
	float buffer[CIRCULAR_BUFFER_SIZE];
	int16_t curr; //indice del ultimo elemento agregado
}circular_buffer_t;
/***********************************************************
 * 				VARIABLES WITH GLOBAL SCOPE
 ***********************************************************/
static circular_buffer_t FSK_signal;
static circular_buffer_t PreFiltered_signal;
static circular_buffer_t result_signal;


static uint32_t fs; //Sample frequency of the FSK signal
static uint8_t prev_samples; //number of samples acorrding to the delay and the fs.
static float fir_coeffs[] = {0.0099524156275609069, 0.025829664997474223, 0.0052301942208776179,
							-0.020566023428597952, -0.050258725765821938, -0.039485305772910186,
							0.030240182765554114, 0.14581109542716533, 0.2548359331518294,
							0.29968551708002261, 0.2548359331518294, 0.14581109542716533,
							0.030240182765554114, -0.039485305772910186, -0.050258725765821938,
							-0.020566023428597952, 0.0052301942208776179, 0.025829664997474223,
							0.0099524156275609069};



/***********************************************************
 * 				LOCAL FUNCTIONS DECLARATIONS
 ***********************************************************/

/**
 * @brief Applys FIR filter and stores the results
 * @param cant Number of values to calculate
*/
void ApplyFIR(uint8_t cant);

/**
 * @brief Stores value of digital signal
 * @param digital_signal
 * @param cant number of samples from analog signal
*/
void ReconstructSignal(bool* digital_signal, uint8_t cant);

/***********************************************************
 * 				FUNCTIONS WITH GLOBAL SCOPE
 ***********************************************************/
void DemodulatorInit(uint32_t fs_)
{
	int i =0;
	fs = fs_;
	FSK_signal.curr = -1;
	PreFiltered_signal.curr = -1;
	result_signal.curr = -1;

	prev_samples = (uint8_t)( DELAY*fs);
	for(i=0; i< CIRCULAR_BUFFER_SIZE; i++)
	{
		result_signal.buffer[i] = 1; //By default the initial state is all '1'.
	}
}

void DemodulateSignal(float* recieved,uint8_t buffer_size)
{
	float aux = 0;
	int i =0;
	uint16_t aux_index = 0;

	//Update FSK Buffer
	for(i=0; i<buffer_size; i++)
	{
		FSK_signal.curr = (FSK_signal.curr+1) % CIRCULAR_BUFFER_SIZE;
		(FSK_signal.buffer)[FSK_signal.curr] = recieved[i];
	}

	//Update PreFiltered Buffer
	for(i=0; i<buffer_size; i++)
	{
		PreFiltered_signal.curr = (PreFiltered_signal.curr+1) % CIRCULAR_BUFFER_SIZE;
		aux =  (FSK_signal.buffer)[PreFiltered_signal.curr];
		if( (PreFiltered_signal.curr - prev_samples) < 0 )
		{
			aux_index = CIRCULAR_BUFFER_SIZE + (PreFiltered_signal.curr - prev_samples);
		}
		else
		{
			aux_index = PreFiltered_signal.curr - prev_samples;
		}
		aux *= (FSK_signal.buffer)[aux_index];
		(PreFiltered_signal.buffer)[PreFiltered_signal.curr] = aux;
	}
	//Update result
	ApplyFIR(buffer_size);


}

void GetData(float* data_buffer,uint8_t num_samples)
{
	int i =0;
	int aux_index = 0;
	for(i=1; i<= num_samples; i++)
	{
		aux_index = result_signal.curr-num_samples+i;
		if( aux_index < 0 )
		{
			aux_index += CIRCULAR_BUFFER_SIZE;
		}
		data_buffer[i] = result_signal.buffer[aux_index];
	}
}

/***********************************************************
 * 				FUNCTIONS WITH LOCAL SCOPE
 ***********************************************************/
void ApplyFIR(uint8_t cant)
{
	int i=0;
	int j=0;
	uint16_t aux_index = 0;
	float aux = 0;
	for(i=0; i< cant; i++)
	{
		aux = 0;
		result_signal.curr = (result_signal.curr+1) % CIRCULAR_BUFFER_SIZE;

		for(j=0; j< FIR_ORDER+1; j++)
		{
			if( (result_signal.curr-j)<0 )
			{
				aux_index = CIRCULAR_BUFFER_SIZE + (result_signal.curr-j);
			}
			else
			{
				aux_index = result_signal.curr-j;
			}
			aux += ( fir_coeffs[j] ) * ( (PreFiltered_signal.buffer)[aux_index] );
		}
		//ApplyComparator
		if( (result_signal.buffer)[(result_signal.curr)-1] == 0)
		{
			if( aux < VLOW )
			{
				(result_signal.buffer)[result_signal.curr] = 1;
			}
			else
			{
				(result_signal.buffer)[result_signal.curr] = 0;
			}
		}
		else
		{
			if( aux > VHIGH )
			{
				(result_signal.buffer)[result_signal.curr] = 0;
			}
			else
			{
				(result_signal.buffer)[result_signal.curr] = 1;
			}
		}
	}
	return;
}

void ReconstructSignal(bool* digital_signal, uint8_t cant);
{

}
