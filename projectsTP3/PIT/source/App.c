/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "bitStreamQueue.h"
#include "PIT.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 *******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */

static void fun(void){
	static int i = 0;
	i++;
	if(i == 5){
		PITmodifyTimer(0, 1000);
	}
	if(i == 10){
		pit_config_t configP1 = { 	1500, 	/* Value of timer. In micro seconds if not in chain mode. In cycles if in chain mode. */
									1, 		/* Number of PIT timer. */
									false,  /* True if timer in Chain Mode. */
									NULL 	/* Callback for interrupt. NULL if interrupt is disabled. */
								};
		PITstartTimer(configP1);
	}
	if(i == 15){
		PITenableTimerInterrupt(1, fun);
	}
	if(i == 20){
		PITstopTimer(1);
	}
}

void App_Init (void)
{
	bitStreamQueueInit();
	PITinit();
	PITinit();
	pit_config_t configP0 = { 	500, 	/* Value of timer. In micro seconds if not in chain mode. In cycles if in chain mode. */
								0, 		/* Number of PIT timer. */
								false,  /* True if timer in Chain Mode. */
								fun 	/* Callback for interrupt. NULL if interrupt is disabled. */
							};
	PITstartTimer(configP0);

}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
