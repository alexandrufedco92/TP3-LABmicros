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
	uint32_t timerVal; /* Value of timer. In micro seconds if not in chain mode. In cycles if in chain mode. */
	uint8_t timerNbr; /* Number of PIT timer. */
	bool chainMode; /* True if timer in Chain Mode. */
	pitFun_t pitCallback; /* Callback for interrupt. NULL if interrupt is disabled. */
}pit_config_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize PIT driver.
 */
void PITinit(void);

/**
 * @brief Modify Timer value. This change will have effect once the timer ends the current countdown.
 * @param timerNbr Timer number to modify.
 * @param value Period of timer in us (microseconds). Value should be between 1us to 80s. If timer is in chain mode
 * then it is equal to number of n+1 timer cycles. In this case, values go from 0x1 to 0xFFFFFFFF.
 */
void PITmodifyTimer(uint8_t timerNbr, uint32_t value);

/**
 * @brief Starts new Timer.
 * @param config Timer configuration (see pit_config_t).
 * @return True if Initialization succeed.
 */
bool PITstartTimer(pit_config_t config);

/**
 * @brief Stops current running timer.
 * @param timerNbr Timer number to modify.
 * @return True if timer stopped successfully.
 */
bool PITstopTimer(uint8_t timerNbr);

/**
 * @brief Enable interrupt for running timer.
 * @param timerNbr Timer number to modify.
 * @param pitCallback Callback for interruption.
 * @return True if Initialization succeed.
 */
bool PITenableTimerInterrupt(uint8_t timerNbr, pitFun_t pitCallback);

/**
 * @brief Disable interrupt for running timer.
 * @param timerNbr Timer number to modify.
 * @return True if timer disabled successfully.
 */
bool PITdisableTimerInterrupt(uint8_t timerNbr);

/*******************************************************************************
 ******************************************************************************/

#endif // _PIT_H_

