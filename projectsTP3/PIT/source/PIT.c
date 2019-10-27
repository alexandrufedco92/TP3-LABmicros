/*******************************************************************************
  @file     PIT.c
  @brief    Periodic Interrupt Timer Driver
  @author   Manuel Mollon
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <PIT.h>
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/* The PIT clock has a frequency of ??? MHz. */
#define PIT_CLK_FREQ	50000000U // Hz
#define CHAIN_MODE_VALUE	0

static bool timerEnabled[MAX_TIMERS_CANT];
static bool timerInChainMode[MAX_TIMERS_CANT];

/*******************************************************************************
 * LOCAL FUNCTION DECLARATIONS
 ******************************************************************************/

/**
 * @brief Enables module Clock.
 * @param base PIT peripheral base address.
 */
static void PITenableClock(PIT_Type * base);

/**
 * @brief Enables PIT.
 * @param base PIT peripheral base address.
 * @param enable True enables module and False disables module.
 */
static void PITenable(PIT_Type * base, bool enable);

/**
 * @brief Initialize individual timer.
 * @param timerNbr Timer number (0-3).
 * @param interruptEnabl .
 * @return Initialization succeed.
 */
static bool PITtimerInit(uint8_t timerNbr, bool interruptEnable);

/**
 * @brief Set individual timer time.
 * @param timerNbr Timer number (0-3).
 * @param value .
 * @return Timer setting succeed.
 */
static bool PITtimerSet(uint8_t timerNbr, uint32_t value);

/*******************************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 ******************************************************************************/

void PITinit (config_t * config){
	/* Saves timer info. Can be modify later if requirements are not met. */
	for(uint8_t i = 0; i < MAX_TIMERS_CANT;i++){
		timerEnabled[i] = config->timerEnable[i];
		timerInChainMode[i] = config->chainMode[i];
	}

	/* Enable clock gating. */
	PITenableClock(PIT);

	/* Enable Module. */
	PITenable(PIT, true); // ver si agregar freeze mode para debug

	/* Set Timers. */
	for(uint8_t i = 0; i < MAX_TIMERS_CANT;i++){
		/* Enables timer if desired. */
		if(timerEnabled[i] == true){
			if(!config->chainMode[i]){ /* If NOT in chain mode. */
				PITtimerSet(i, config->timerVal[i]);
			}
		}
	}

	if(timerInChainMode[0]){ /* First timer can't be in chain mode. */
		timerEnabled[0] = false;
		timerInChainMode[0] = false;
	}

	/* Enable Chain Mode. */
	for(uint8_t i = 1; i < MAX_TIMERS_CANT;i++){
		if((timerInChainMode[i] == true) && (timerEnabled[i] == true) && (timerEnabled[i-1] == true)){
			PITtimerSet(i, CHAIN_MODE_VALUE);
		}
	}

	/* Enable Timers. */
	for(uint8_t i = 0; i < MAX_TIMERS_CANT;i++){
		/* Enables timer if desired. */
		if(timerEnabled[i] == true){
			PITtimerInit(i, config->interruptEnable[i]);
		}
	}
}

void PITmodifyTimer(uint8_t timerNbr, uint32_t value){
	if((timerInChainMode[timerNbr] == false) && (timerEnabled[timerNbr] == true)){
		PITtimerSet(timerNbr, value);
	}
}

/*******************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

static void PITenableClock(PIT_Type * base){
	if(base == PIT){
		/* Set PIT Clock Gate Control value in 1, enabling clock. */
		SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	}
}

static void PITenable(PIT_Type * base, bool enable){
	if(enable){
		base->MCR &= ~PIT_MCR_MDIS_MASK;
	}
	else{
		base->MCR |= PIT_MCR_MDIS_MASK;
	}
}

bool PITtimerInit(uint8_t timerNbr, bool interruptEnable){;
	if(interruptEnable){
		/* Enable Timer 1 interrupts. */
		PIT->CHANNEL[timerNbr].TCTRL = TIE;
	}
	/* Start Timer 1. */
	PIT->CHANNEL[timerNbr].TCTRL |= TEN;
}

bool PITtimerSet(uint8_t timerNbr, uint32_t tValue){
	if(tValue == CHAIN_MODE_VALUE){
		/* Setup chain mode. */
		PIT->CHANNEL[timerNbr].TCTRL |= CHN;
	}
	else{
		/* Setup timer value. */
		uint32_t LDVALvalue = (tValue*PIT_CLK_FREQ)-1; /* LDVAL trigger = (period / clock period) -1 */
		PIT->CHANNEL[timerNbr].LDVAL = LDVALvalue;
	}

}
