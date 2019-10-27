/*******************************************************************************
  @file     PIT.h
  @brief    Periodic Interrupt Timer Driver
  @author   Manuel Mollon
 ******************************************************************************/

#ifndef _PIT_H_
#define _PIT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_TIMERS_CANT	4

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct{
	uint16_t timerVal[MAX_TIMERS_CANT];
	bool interruptEnable[MAX_TIMERS_CANT];
	bool timerEnable[MAX_TIMERS_CANT];
	bool chainMode[MAX_TIMERS_CANT];
}config_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize PIT driver.
 * @param config Configuration details (see config_t).
 */
void PITinit(config_t * config);

/**
 * @brief Initialize PIT driver.
 * @param config Configuration details (see config_t).
 */
void PITmodifyTimer(uint8_t timerNbr, uint32_t value);

//TERMINAR:
// PARSEO
// INTERRUPCIONES
// DEBUGEO

/*******************************************************************************
 ******************************************************************************/

#endif // _PIT_H_

