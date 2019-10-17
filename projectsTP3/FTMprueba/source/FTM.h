/*
 * FTM.h
 *
 *  Created on: 16 oct. 2019
 *      Author: G5
 */

#ifndef FTM_H_
#define FTM_H_

#define NUMBER_OF_FTM_MODULES 4
#define FTM_MAX_PRESCALER 128
#define FTM_MIN_PRESCALER 1
#define FTM_DEFAULT_PRESCALER FTM_MIN_PRESCALER

typedef enum {FTM_TIMER, FTM_INPUT_CAPTURE, FTM_OUTPUT_COMPARE, FTM_PWM}FTMmodes;
typedef enum {FTM0_INDEX, FTM1_INDEX, FTM2_INDEX, FTM3_INDEX}FTMmodules;
typedef enum {FTM_PSCX1, FTM_PSCX2, FTM_PSCX4, FTM_PSCX8, FTM_PSCX16, FTM_PSCX32,
				FTM_PSCX64, FTM_PSCX128}FTMprescaler;
typedef enum {UP_COUNTER, UP_DOWN_COUNTER, FREE_COUNTER, QUAD_PHASE_COUNTER}FTM_TIMERcountModes;

typedef void (*FTM_callback_t)(void);

typedef struct{
	FTMmodules nModule; //FTM0_INDEX or FTM1_INDEX, etc
	FTMmodes mode; //FTM_TIMER, FTM_INPUT_CAPTURE, etc
	FTMprescaler prescaler; //x1, x2, x4, etc
	FTM_TIMERcountModes countMode; //only necessary to specify in FTM_TIMER mode
	FTM_callback_t p2callback;   //pointer to subroutine called when FTM interrupts

	int numOverflows; //quantity of overflows before interrupt
	int nTicks; //count limit of FTM counter
}FTMconfig_t;


void FTMinit(FTMconfig_t * p2config);





#endif /* FTM_H_ */
