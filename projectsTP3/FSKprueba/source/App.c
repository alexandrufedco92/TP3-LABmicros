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
//#include "comController2pc.h"
#include "uart.h"
#include "waveGen.h"
#include "timer.h"
#include <math.h>
#include <stdint.h>
#include "gpio.h"
#include "ADC.h"
#include "FSK_Demodulator.h"
#include "PIT.h"
#include "measureFreq.h"
#include "bitStreamQueue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MARK_KEY '1'
#define SPACE_KEY '0'

#define IS_MARK_FREQ(f) ( (f >= 1000) && (f <= 1400) )
#define IS_SPACE_FREQ(f) ( (f >= 2200) && (f <= 2600) )

#define ADC_SAMPLE_PERIOD 83 //Sample time of ADC in microseconds.

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void FMcallback(void);
float  ShapeAnalogSample( ADC_Data_t sample);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
enum{debugV1, debugV2};

int debugFlag = debugV1;

//FM demodulation
static bool frame_to_send[FRAME_SIZE];
//DAC


//INPUT CAPTURE
int freq = 0;
//UART

char key = MARK_KEY;

/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{
	/*
	uart_cfg_t config;
	config.baudRate = 9600;
	config.nBits = 8;
	config.parity = NO_PARITY_UART;
	config.rxWaterMark = 5;
	config.txWaterMark = 2;
	config.mode = NON_BLOCKING_SIMPLE;

	uartInit (U0, config);
	*/

	if(debugFlag == debugV1)
	{
		WaveGenConfig_t waveConf;
		waveConf.freq = 1200;
		waveConf.id = WAVE0_WAVEGEN;
		waveConf.mode = SAMPLES_WAVEGEN;
		waveConf.waveName = SIN_WAVEGEN;
		initWaveGen(&waveConf);
		InitializeTimers();
		SetTimer(MODULATION, 833, FMcallback);

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
		//FSK Demodulator init
		DemodulatorInit();
		//Initialize PIT1 as ADC Trigger
		PITmodifyTimer(1, ADC_SAMPLE_PERIOD);


	}
	else if(debugFlag == debugV2)
	{
		initFreqMeasure();

		WaveGenConfig_t waveConf;
		waveConf.freq = 1200;
		waveConf.id = WAVE0_WAVEGEN;
		waveConf.mode = PWM_WAVEGEN;
		waveConf.waveName = SIN_WAVEGEN;
		initWaveGen(&waveConf);
	}





}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	if(debugFlag == debugV2)
	{
		//RX
		if(isNewMeasReady())
		{
			freq = getFreqMeasure();
		}
		if(IS_MARK_FREQ(freq))
		{
			key = MARK_KEY;
		}
		else if(IS_SPACE_FREQ(freq))
		{
			key = SPACE_KEY;
		}
		uartWriteMsg(U0, &key, 1);

		//TX  (por ahora se debuggea actualizando con interrupciones)

	}
	else if(debugFlag == debugV1)
	{
		float sample = 0;
		float digital_symbol = 0;
		//RX
		if ( IsConversionFinished() )
		{
			sample = ShapeAnalogSample( GetConversionResult());
			digital_symbol = DemodulateSignal( sample );
			if( digital_symbol != -1)
			{
				PushBit(digital_symbol);
			}
			if( IsFrameReady() )
			{
				GetFrame( frame_to_send );//Mando el frame por Uart.
			}

		}

		//TX (por ahora se debuggea actualizando con interrupciones)

	}


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


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void FMcallback(void)
{
	static int i = 2;
	if((i % 2) == 0)
	{
		i = 3;
		updateWaveFreq(WAVE0_WAVEGEN, 1200);

	}
	else
	{
		i = 2;
		updateWaveFreq(WAVE0_WAVEGEN, 2200);
	}
}

float  ShapeAnalogSample( ADC_Data_t analog_sample)
{
	float sample = 2*analog_sample;
	return ( sample/( (float)UINT16_MAX )-1 );
}

/*******************************************************************************
 ******************************************************************************/
