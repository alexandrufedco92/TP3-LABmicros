/*
 * FSK_Demodulator.c
 *
 *  Created on: Oct 19, 2019
 *      Author: G5
 */

#include "FSK_Demodulator.h"
#include "PIT.h"
#include "gpio.h"	//Solo para Debuggear.
/***********************************************************
 *					 DEFINES AND MACROS
 ***********************************************************/
#define FL 1200	//Frequency of a logical '1'
#define FH 2200	//Frequency of a logical '0'
#define DELAY 0.0004464 //Delay optimo en s
#define F_SAMPLE 12000 //Sample frecuency of 12KHz
#define T_SAMPLE_PERIOD 83 //Sample time of ADC in microseconds.
#define VLOW -0.25
#define VHIGH 0.25
#define FIR_ORDER 18
//Reconstruction parameters
#define AVG_SAMPLES 10 //Number of samples to average to decide '1' or '0'.
#define MID 0.5				//Treshold for deciding between '0' and '1'.
#define IDLE_LIMIT 8		//Nummber of '1' symbols in a row that start idle state.

//Size of Buffers
#define CIRCULAR_BUFFER_SIZE 40

typedef struct{
	float buffer[CIRCULAR_BUFFER_SIZE];
	int16_t curr; //indice del ultimo elemento agregado
}circular_buffer_t;
/***********************************************************
 * 				VARIABLES WITH GLOBAL SCOPE
 ***********************************************************/
static circular_buffer_t FSK_signal;
static circular_buffer_t PreFiltered_signal;
static float last_fir_output = 1;
static float last_sample_value;		//Last digital value recieved
static float average_aux = 0;		//Average of Comparatpr samples.
static uint8_t sample_counter = 0;	//Counts number of samples averaged.
static uint8_t idle_counter = 0;	//Counter that indicates if in idle state.
static bool idle = true;		//Flag that indicates if in idle state
static bool symbol_detected = false;//Flag that indicates when a digital symbol was detected.

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
float ApplyFIR(void);

/**
 * @brief Stores value of digital signal
 * @param digital_signal
 * @param cant number of samples from analog signal
*/
bool ReconstructSignal(float comp_out);

/***********************************************************
 * 				FUNCTIONS WITH GLOBAL SCOPE
 ***********************************************************/
void DemodulatorInit(void)
{
	fs = F_SAMPLE;
	FSK_signal.curr = -1;
	PreFiltered_signal.curr = -1;

	prev_samples = (uint8_t)( DELAY*fs);
	last_sample_value = 1;
	//Pit initializaion for ADC sampling
	pit_config_t pit_config;
	pit_config.timerVal = T_SAMPLE_PERIOD;
	pit_config.timerNbr = 1;
	pit_config.chainMode = false;
	pit_config.pitCallback = NULL;
	PITinit();
	PITstartTimer(&pit_config);

}

bool DemodulateSignal(float recieved)
{
	float aux = 0;
	uint16_t aux_index = 0;
	float comp_out = 0; //Comparator output

	//Update FSK Buffer
	FSK_signal.curr = (FSK_signal.curr+1) % CIRCULAR_BUFFER_SIZE;
	(FSK_signal.buffer)[FSK_signal.curr] = recieved;

	//Update PreFiltered Buffer
	PreFiltered_signal.curr = (PreFiltered_signal.curr+1) % CIRCULAR_BUFFER_SIZE;
	aux =  recieved;
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

	//Update result
	comp_out = ApplyFIR();
	//Gets bitstream values.
	return ReconstructSignal(comp_out);
}

bool IsDemodulationFinished(void)
{
	return symbol_detected;
}

/***********************************************************
 * 				FUNCTIONS WITH LOCAL SCOPE
 ***********************************************************/
float ApplyFIR(void)
{
	int j=0;
	uint16_t aux_index = 0;
	float aux = 0;


	for(j=0; j< FIR_ORDER+1; j++)
	{
		if( (PreFiltered_signal.curr-j)<0 )
		{
			aux_index = CIRCULAR_BUFFER_SIZE + (PreFiltered_signal.curr-j);
		}
		else
		{
			aux_index = PreFiltered_signal.curr-j;
		}
		aux += ( fir_coeffs[j] ) * ( (PreFiltered_signal.buffer)[aux_index] );
	}
	//ApplyComparator
	if( last_fir_output == 0)
	{
		if( aux < VLOW )
		{
			last_fir_output = 1;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if( aux > VHIGH )
		{
			last_fir_output = 0;
			return 0;
		}
		else
		{
			return 1;
		}
	}

	return;
}

bool ReconstructSignal(float comp_out)
{
	bool result = false;
	if(idle)
	{
		if( comp_out)
		{
			if(++sample_counter == AVG_SAMPLES)
			{
				sample_counter = 0;
				symbol_detected = false;
			}
			else
			{
				symbol_detected = false;
			}
		}
		else
		{
			idle = false;
			idle_counter = 0;
			sample_counter = 1;
			symbol_detected = false;
		}
	}
	else
	{
		average_aux += comp_out;
		if( (++sample_counter) == AVG_SAMPLES)
		{
			sample_counter = 0;
			average_aux /= AVG_SAMPLES;
			if( average_aux >= MID )
			{
				last_sample_value = 1;
				if( (++idle_counter) == IDLE_LIMIT )
					idle = true;
				average_aux = 0;
				symbol_detected = true;
				result = true;
			}
			else
			{
				last_sample_value = 0;
				idle_counter = 0;
				average_aux = 0;
				symbol_detected = true;
				result = false;
			}
		}
		else
		{
			symbol_detected = false;
		}

	}

	return result;


}
