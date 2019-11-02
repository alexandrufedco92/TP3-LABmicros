/*
 * FSK_Demodulator.c
 *
 *  Created on: Oct 19, 2019
 *      Author: G5
 */

#include "FSK_Demodulator.h"
#include "ADC.h"
#include "bitStreamQueue.h"
#include "measureFreq.h"

/***********************************************************
 *					 DEFINES AND MACROS
 ***********************************************************/
#define FL 1200	//Frequency of a logical '1'
#define FH 2200	//Frequency of a logical '0'
#define DELAY 0.0004464 //Delay optimo en s
#define F_SAMPLE 12000 //Sample frecuency of 12KHz
#define VLOW -0.25
#define VHIGH 0.25
#define FIR_ORDER 18
//Reconstruction parameters
#define AVG_SAMPLES 10 //Number of samples to average to decide '1' or '0'.
#define MID 5				//Treshold for deciding between '0' and '1'.

//Size of Buffers
#define CIRCULAR_BUFFER_SIZE 100

#define MARK_KEY '1'
#define SPACE_KEY '0'

#define IS_MARK_FREQ(f) ( (f >= 1000) && (f <= 1400) )
#define IS_SPACE_FREQ(f) ( (f >= 2200) && (f <= 2600) )

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
static float average_aux = 0;		//Average of Comparator samples.
static uint8_t sample_counter = 0;	//Counts number of samples averaged.
static bool idle = true;		//Flag that indicates if in idle state

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
void ReconstructSignal(float comp_out);
void InitializeHardware(void);

/***********************************************************
 * 				FUNCTIONS WITH GLOBAL SCOPE
 ***********************************************************/
void DemodulatorInit(void)
{
#ifdef DSP_VERSION //DSP_VERSION
	fs = F_SAMPLE;
	FSK_signal.curr = -1;
	PreFiltered_signal.curr = -1;
	prev_samples = (uint8_t)( DELAY*fs);
	last_sample_value = 1;
	InitializeHardware();

#else	//PWM_VERSION
	initFreqMeasure();
#endif

}

void DemodulateSignal(void)
{
#ifdef DSP_VERSION
	float aux = 0;
	float recieved;

	recieved = GetConversionResult();
	recieved *= 2;
	//Turn ADC result to float value ranging from -1 to 1.
	recieved /= (float)(UINT16_MAX);
	recieved -= 1;
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
	ReconstructSignal(comp_out);
#else
	int freq = 0;
	freq = getFreqMeasure( );
	if( IS_MARK_FREQ(freq) && (!idle))
	{
		PushBit(MARK_KEY);
	}
	else if( IS_SPACE_FREQ(freq) )
	{
		idle = false;
		PushBit(SPACE_KEY);
	}
	if(IsFrameReady())
	{
		idle = true;
	}
#endif

}


bool NeedDemodulation(void)
{
	#ifdef DSP_VERSION
		return IsConversionFinished();
	#else	//PWM_VERSION
		return isNewMeasReady();
	#endif
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

}

void ReconstructSignal(float comp_out)
{
	if(idle)
	{
		if(!comp_out)
		{
			idle = false;
			sample_counter = 1;
		}
	}
	else
	{
		average_aux += comp_out;
		if( (++sample_counter) == AVG_SAMPLES)
		{
			sample_counter = 0;
			if( average_aux >= MID )
			{
				last_sample_value = 1;
				average_aux = 0;
				PushBit('1'); //Adds a '1' to the frame.
			}
			else
			{
				last_sample_value = 0;
				average_aux = 0;
				PushBit('0'); //Adds a '0' to the frame.
			}
			if( IsFrameReady() )
				idle = true;
		}

	}

}

void InitializeHardware(void)
{
	//Set ADC configuration
	ADC_Config_t adc_config;
	adc_config.channel_sel = AD0;
	adc_config.clock_divide = DIVIDE_BY_1;
	adc_config.clock_type = BUS_CLOCK;
	adc_config.diffential_mode = false;
	adc_config.enable_cont_conversions = false;
	adc_config.enable_hardware_avg = false;
	adc_config.enable_interrupts = false;
	adc_config.id = FIRST_ADC;
	adc_config.low_power = true;
	adc_config.resolution = SIXTEEN_BITS;
	adc_config.trigger = HARDWARE_TRIGGER;
	adc_config.voltage_reference = DEFAULT;
	ADC_Init( &adc_config);

}


