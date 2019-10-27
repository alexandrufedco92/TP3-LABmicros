/*
 * ADC.h
 *
 *  Created on: Oct 23, 2019
 *      Author: G5
 */

#ifndef ADC_H_
#define ADC_H_

/************************************************************
 * 					HEADERS INCLUDED
 ************************************************************/
#include <stdbool.h>
#include <stdint.h>

/************************************************************
 * 				DEFINITIONS AND DATA TYPES
 ************************************************************/
typedef enum{
			BUS_CLOCK = 0X00,	//Bus Clock of 50MHz.
			BUS_CLOCK2 = 0X01,	//Bus Clock divided by 2.
			ALT_CLK = 0X02,		//External Clock
			ASYNC_CLK = 0X03	//Async Clock generated by the ADC
			}ADC_Clock_t;

typedef enum{
			DIVIDE_BY_1 = 0X00,
			DIVIDE_BY_2 = 0X01,
			DIVIDE_BY_4 = 0X02,
			DIVIDE_BY_8 = 0X03
}ADC_ClockDiv_t;

typedef enum{
			EIGHT_BITS = 0X00,
			TWELVE_BITS = 0X01,
			TEN_BITS = 0X02,
			SIXTEEN_BITS = 0X03
}ADC_Resolution_t;

typedef enum{
			SOFTWARE_TRIGGER  = 0X00,
			HARDWARE_TRIGGER = 0X01
}ADC_Trigger_t;

typedef enum{
	DEFAULT = 0X00,
	ALTERNATE = 0X01
}ADC_VoltageRef_t;

typedef enum{
	FOUR_SAMPLES = 0X00,
	EIGHT_SAMPLES = 0X01,
	SIXTEEN_SAMPLES =0X02,
	THIRTY_TWO_SAMPLES = 0X03
}ADC_AverageSelect_t;

typedef enum{
	SHORT_SAMPLE_TIME = 0x00,
	LONG_SAMPLE_TIME = 0X01
}ADC_SampleTime_t;

typedef enum{
	AD0 = 0X00, AD1, AD2, AD3, AD4, AD5, AD6, AD7, AD8, AD9,
	AD10, AD11, AD12, AD13, AD14, AD15, AD16, AD17, AD18,
	AD19, AD20, AD21, AD22, AD23
}ADC_Channel_t;

typedef enum{
	FIRST_ADC=0,	//ID for ADC0
	SECOND_ADC=1	//ID for ADC1
}ADC_ID_t;


typedef struct{
	ADC_ID_t id;
	ADC_Channel_t channel_sel;
	ADC_Resolution_t resolution;
	ADC_Clock_t clock_type;
	ADC_ClockDiv_t clock_divide;
	ADC_SampleTime_t sample_time;
	bool low_power;
	bool diffential_mode;
	bool enable_interrupts;
	bool enable_cont_conversions;
	bool enable_hardware_avg;
	ADC_AverageSelect_t samples_to_average;
	ADC_Trigger_t trigger;
	ADC_VoltageRef_t voltage_reference;
}ADC_Config_t;

typedef int16_t ADC_Data_t;

/*************************************************************
 * 					HEADER FUNCTIONS
 *************************************************************/

/*
 * @brief Initializes the ADC peripheric.
 * @param config contains all the information needed for initialization.
 * @return true if initialization was succesful, false otherwise.
 */
bool ADC_Init( const ADC_Config_t* config);

bool ADC_Calibrate(void);

bool StartConversion(ADC_Channel_t channel, bool interrupt_enable);

bool IsConversionFinished(void);

ADC_Data_t GetConversionResult(void);

void SetVoltageReference(ADC_VoltageRef_t);



#endif /* ADC_H_ */