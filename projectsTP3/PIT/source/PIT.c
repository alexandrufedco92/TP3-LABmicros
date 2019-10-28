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

/* The PIT clock has a frequency of 50MHz. */
#define PIT_CLK_FREQ	50000000U // Hz
#define FREQ_SCALE	1000000U // 1MHz
/* Max and Min values when not in chain mode.*/
#define MAX_VALUE	80000000UL
#define MIN_VALUE	1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

static bool timerEnabled[MAX_TIMERS_CANT];
static bool timerInChainMode[MAX_TIMERS_CANT];
static pitFun_t pitFuns[MAX_TIMERS_CANT];

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
static void PITenable(PIT_Type * base, bool enable, bool freezeMode);

/**
 * @brief Initialize individual timer.
 * @param timerNbr Timer number (0-3).
 * @param interruptEnable True to trigger interrupt at the end of timer. False disables interruptes.
 * @return Initialization succeed.
 */
static bool PITtimerInit(uint8_t timerNbr, bool interruptEnable);

/**
 * @brief Set individual timer time.
 * @param timerNbr Timer number (0-3).
 * @param value Period in microseconds. If in chain mode, then number of cycles.
 * @param chainMode True to use chain mode, False if chain mode not used.
 * @return Timer setting succeed.
 */
static bool PITtimerSet(uint8_t timerNbr, uint32_t value, bool chainMode);

/**
 * @brief Handler for the PIT0 interruption.
 */
void PIT0_IRQHandler(void);

/**
 * @brief Handler for the PIT1 interruption.
 */
void PIT1_IRQHandler(void);

/**
 * @brief Handler for the PIT2 interruption.
 */
void PIT2_IRQHandler(void);

/**
 * @brief Handler for the PIT3 interruption.
 */
void PIT3_IRQHandler(void);


/*******************************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 ******************************************************************************/

void PITinit(config_t * config){
	/* Saves timer info. Can be modify later if requirements are not met. */
	for(uint8_t i = 0; i < MAX_TIMERS_CANT;i++){
		timerEnabled[i] = config->timerEnable[i];
		timerInChainMode[i] = config->chainMode[i];
		pitFuns[i] = config->pitCallbacks[i];
	}

	/* Enable clock gating. */
	PITenableClock(PIT);

	/* Enable Module. */
	PITenable(PIT, true, true);

	/* Set Timers that are not in chain mode. */
	for(uint8_t i = 0; i < MAX_TIMERS_CANT;i++){
		/* Sets timer if desired. */
		if((timerEnabled[i] == true) && (timerInChainMode[i] == false)){
			if(PITtimerSet(i, config->timerVal[i], false)){
				/* Timer set successfully. */
			}
			else{
				timerEnabled[i] = false;
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
			PITtimerSet(i, config->timerVal[i], true);
		}
		else if(timerInChainMode[i] == true){
			timerEnabled[i] = false;
			timerInChainMode[i] = false;
		}
	}

	/* Enable Timers. */
	for(uint8_t i = 0; i < MAX_TIMERS_CANT;i++){
		/* Enables timer if desired. */
		if(timerEnabled[i] == true){
			PITtimerInit(i, config->interruptEnable[i]);
		}
	}

	/* Enable Interrupts. */
	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT1_IRQn);
	NVIC_EnableIRQ(PIT2_IRQn);
	NVIC_EnableIRQ(PIT3_IRQn);
}

void PITmodifyTimer(uint8_t timerNbr, uint32_t value){
	if(timerEnabled[timerNbr] == true){
		PITtimerSet(timerNbr, value,timerInChainMode[timerNbr]);
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

static void PITenable(PIT_Type * base, bool enable, bool freezeMode){
	if(enable){
		base->MCR &= ~PIT_MCR_MDIS_MASK;
		if(freezeMode){
			/* Freeze Mode to debug. */
			base->MCR |= PIT_MCR_FRZ_MASK;
		}
	}
	else{
		base->MCR |= PIT_MCR_MDIS_MASK;
	}
}

bool PITtimerInit(uint8_t timerNbr, bool interruptEnable){
	if(timerNbr>3){ /* Timer Number must be valid. */
		return false;
	}
	if(interruptEnable){
		/* Enable Timer interrupts. */
		PIT->CHANNEL[timerNbr].TCTRL |= PIT_TCTRL_TIE_MASK;
	}
	/* Start Timer. */
	PIT->CHANNEL[timerNbr].TCTRL |= PIT_TCTRL_TEN_MASK;
	return true;
}

bool PITtimerSet(uint8_t timerNbr, uint32_t tValue, bool chainMode){
	/* Timer Number must be valid and value between limits. */
	if(timerNbr>3){
		return false;
	}
	if((!timerInChainMode[timerNbr]) && ((tValue < MIN_VALUE) || (tValue > MAX_VALUE))){
		return false;
	}
	uint32_t LDVALvalue;
	if(chainMode){
		/* Setup chain mode. */
		PIT->CHANNEL[timerNbr].TCTRL |= PIT_TCTRL_CHN_MASK;
		LDVALvalue = tValue;
	}
	else{
		/* Setup timer value. */
		LDVALvalue = (tValue*(PIT_CLK_FREQ/FREQ_SCALE))-1; /* LDVAL trigger = (period / clock period) -1 */
	}
	PIT->CHANNEL[timerNbr].LDVAL = LDVALvalue;
	return true;
}

void PIT0_IRQHandler(void){
	/* Clears interrupt flag. */
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;

	if(pitFuns[0] != NULL){
		pitFuns[0](); 	/* Callback for PIT0. */
	}
}

void PIT1_IRQHandler(void){
	/* Clears interrupt flag. */
	PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;

	if(pitFuns[1] != NULL){
		pitFuns[1]();  /* Callback for PIT1. */
	}
}

void PIT2_IRQHandler(void){
	/* Clears interrupt flag. */
	PIT->CHANNEL[2].TFLG |= PIT_TFLG_TIF_MASK;

	if(pitFuns[2] != NULL){
		pitFuns[2]();  /* Callback for PIT2. */
	}
}

void PIT3_IRQHandler(void){
	/* Clears interrupt flag. */
	PIT->CHANNEL[3].TFLG |= PIT_TFLG_TIF_MASK;

	if(pitFuns[3] != NULL){
		pitFuns[3]();  /* Callback for PIT3. */
	}
}



