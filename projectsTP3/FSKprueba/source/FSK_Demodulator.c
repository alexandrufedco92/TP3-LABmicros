/*
 * FSK_Demodulator.c
 *
 *  Created on: Oct 19, 2019
 *      Author: G5
 */

#include "FSK_Demodulator.h"
/***********************************************************
 *					 DEFINES AND MACROS
 ***********************************************************/
#define FL 1200	//Frequency of a logical '1'
#define FH 2200	//Frequency of a logical '0'
#define DELAY 0.0004464 //Delay optimo en s
//Size of Buffers
#define CIRCULAR_BUFFER_SIZE 50

typedef struct{
	static int16_t buffer[CIRCULAR_BUFFER_SIZE];
	uint16_t curr; //indice del ultimo elemento agregado
}circular_buffer_t;
/***********************************************************
 * 				VARIABLES WITH GLOBAL SCOPE
 ***********************************************************/
static circular_buffer_t FSK_signal;
static circular_buffer_t PreFiltered_signal;
static circular_buffer_t result_signal;


static uint32_t fs; //Sample frequency of the FSK signal
static uint8_t prev_samples; //number of samples acorrding to the delay and the fs.

/***********************************************************
 * 				LOCAL FUNCTIONS DECLARATIONS
 ***********************************************************/
void ApplyFIR(void);

/***********************************************************
 * 				FUNCTIONS WITH GLOBAL SCOPE
 ***********************************************************/
void DemodulatorInit(uint32_t fs_)
{
	fs = fs_;
	FSK_signal.curr = 0;
	PreFiltered_signal.curr = 0;
	result_signal.curr = 0;

	prev_samples = (uint8_t)( DELAY*fs);
}

void DemodulateSignal(int16_t* recieved,uint8_t buffer_size)
{
	int16_t aux = 0;
	int i =0;
	uint16_t aux_index = 0;

	//Update FSK Buffer
	for(i=0; i<buffer_size; i++)
	{
		FSK_signal.curr = (FSK_signal.curr+1) % CIRCULAR_BUFFER_SIZE;
		(FSK_signal.buffer)[FSK_signal.curr] = recieved[i];
	}

	//Udate PreFiltered Buffer
	for(i=0; i<buffer_size; i++)
	{
		PreFiltered_signal.curr = (PreFiltered_signal.curr+1) % CIRCULAR_BUFFER_SIZE;
		aux =  (FSK_signal.buffer)[FSK_signal.curr];
		if( (FSK_signal.curr - prev_samples) < 0 )
		{
			aux_index = CIRCULAR_BUFFER_SIZE + (FSK_signal.curr - prev_samples);
		}
		else
		{
			aux_index = FSK_signal.curr - prev_samples;
		}
		aux *= (FSK_signal.buffer)[aux_index];
		(PreFiltered_signal.buffer)[PreFiltered_signal.curr] = aux;
	}
	//Update result
	ApplyFIR();


}

/***********************************************************
 * 				FUNCTIONS WITH LOCAL SCOPE
 ***********************************************************/
void ApplyFIR(void)
{

}
