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
static bool pitInitDone = false;

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
 * @brief Disables timer.
 * @param timerNbr Timer number (0-3).
 * @param timerDisable True to disable timer.
 * @param interruptDisable True to disable interrupt.
 * @return Disable succeed.
 */
static bool PITtimerDisable(uint8_t timerNbr, bool timerDisable, bool interruptDisable);

/**
 * @brief Initialize individual timer interruption.
 * @param timerNbr Timer number (0-3).
 * @return Initialization succeed.
 */
static bool PITtimerInterrupt(uint8_t timerNbr);

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

void PITinit(void){
	if(pitInitDone){
		return;
	}

	/* Enable clock gating. */
	PITenableClock(PIT);

	/* Enable Module. */
	PITenable(PIT, true, true);

	/* Enable Interrupts. */
	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT1_IRQn);
	NVIC_EnableIRQ(PIT2_IRQn);
	NVIC_EnableIRQ(PIT3_IRQn);

	for(int i = 0; i<MAX_TIMERS_CANT; i++){
		timerEnabled[i] = false;
		timerInChainMode[i] = false;
		pitFuns[i] = NULL;
	}

	pitInitDone = true;
}

void PITmodifyTimer(uint8_t timerNbr, uint32_t value){
	if(timerEnabled[timerNbr] == true){
		PITtimerSet(timerNbr, value,timerInChainMode[timerNbr]);
	}
}

bool PITstartTimer(pit_config_t config){
	/* Set Timer if not in chain mode. */
	if(config.chainMode == false){
		if(PITtimerSet(config.timerNbr, config.timerVal, false)){
			/* Timer set successfully. */
			timerEnabled[config.timerNbr] = true;
			timerInChainMode[config.timerNbr] = false;
		}
		else{
			timerEnabled[config.timerNbr] = false;
		}

	}

	if(config.chainMode && (config.timerNbr == 0)){ /* First timer can't be in chain mode. */
		timerEnabled[0] = false;
		timerInChainMode[0] = false;
	}
	else if(config.chainMode && timerEnabled[config.timerNbr-1]){ /* Enable Chain Mode. */
		PITtimerSet(config.timerNbr, config.timerVal, true);
	}
	else if(config.chainMode){
		timerEnabled[config.timerNbr] = false;
		timerInChainMode[config.timerNbr] = false;
	}

	/* Enable Timer. */
	if(timerEnabled[config.timerNbr]){
		bool interruptEnable = false;
		if(config.pitCallback != NULL){
			interruptEnable = true;
			pitFuns[config.timerNbr] = config.pitCallback;
		}
		PITtimerInit(config.timerNbr, interruptEnable);
	}
	return timerEnabled[config.timerNbr];
}

bool PITstopTimer(uint8_t timerNbr){
	bool successStatus = false;
	successStatus = PITtimerDisable(timerNbr, true, true);
	if(successStatus){
		timerEnabled[timerNbr] = false;
		pitFuns[timerNbr] = NULL;
	}
	return successStatus;
}

bool PITenableTimerInterrupt(uint8_t timerNbr, pitFun_t pitCallback){
	bool successStatus = false;
	if(timerEnabled[timerNbr]){ /* Only enable interrupt if timer is enabled. */
		successStatus = PITtimerInterrupt(timerNbr);
	}
	if(successStatus){
		pitFuns[timerNbr] = pitCallback;
	}

	return successStatus;
}

bool PITdisableTimerInterrupt(uint8_t timerNbr){
	bool successStatus = false;
	successStatus = PITtimerDisable(timerNbr, false, true);
	if(successStatus){
		pitFuns[timerNbr] = NULL;
	}

	return successStatus;
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

static bool PITtimerInit(uint8_t timerNbr, bool interruptEnable){
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

static bool PITtimerDisable(uint8_t timerNbr, bool timerDisable, bool interruptDisable){
	if(timerNbr>3){ /* Timer Number must be valid. */
		return false;
	}
	if(interruptDisable){
		/* Disable Timer interrupts. */
		PIT->CHANNEL[timerNbr].TCTRL &= ~PIT_TCTRL_TIE_MASK;
	}
	if(timerDisable){
		/* Stop Timer. */
		PIT->CHANNEL[timerNbr].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	}
	return true;
}

static bool PITtimerInterrupt(uint8_t timerNbr){
	if(timerNbr>3){ /* Timer Number must be valid. */
		return false;
	}
	/* Enable Timer interrupts. */
	PIT->CHANNEL[timerNbr].TCTRL |= PIT_TCTRL_TIE_MASK;
	return true;
}

static bool PITtimerSet(uint8_t timerNbr, uint32_t tValue, bool chainMode){
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



