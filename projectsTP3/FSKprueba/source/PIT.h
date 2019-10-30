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
#define NULL 0L

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef void (*pitFun_t)(void);

typedef struct{
	uint32_t timerVal[MAX_TIMERS_CANT];
	bool interruptEnable[MAX_TIMERS_CANT];
	bool timerEnable[MAX_TIMERS_CANT];
	bool chainMode[MAX_TIMERS_CANT];
	pitFun_t pitCallbacks[MAX_TIMERS_CANT];
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
 * @brief Modify Timer value. This change will have effect once the timer ends the current countdown.
 * @param timerNbr Timer number to modify.
 * @param value Period of timer in us (microseconds). Value should be between 1us to 80s. If timer is in chain mode
 * then it is equal to number of n+1 timer cycles. In this case, values go from 0x1 to 0xFFFFFFFF.
 */
void PITmodifyTimer(uint8_t timerNbr, uint32_t value);

/*******************************************************************************
 ******************************************************************************/

#endif // _PIT_H_

