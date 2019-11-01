/*
 * PDB.c
 *
 *  Created on: Oct 30, 2019
 *      Author: Lu
 */
#include "MK64F12.h"
#include "PDB.h"


#define BUS_CLOCK 					50000000U
#define	PDB_LOAD_VALUE_DEF			0
#define PDB_PRESCALER_DEF 			0
#define PDB_DIV_MULT_DEF			0
#define PDB_TRIG_INPUT_DEF			15
#define PDB_DELAY_VALUE_DEF			1000U
#define PDB_MOD_VALUE_DEF 			1000U
#define PDB_DAC_INTERVAL_VALUE_DEF 	BUS_CLOCK/(1200*16)
#define PDB_DAC_CH_DEF				0

static void clockGating(void);
static void setModulusValue(uint32_t);
static void setCounterDelayValue(uint32_t);

void initPDB(pdb_config_t* conf){

	clockGating();

	PDB0->SC |= PDB_SC_PDBEN_MASK;
	uint32_t mask;	//me quedo con el valor del SC antes, y borro lo que configuraré
	mask = PDB0->SC & ~(PDB_SC_LDMOD_MASK | PDB_SC_PRESCALER_MASK | PDB_SC_TRGSEL_MASK | PDB_SC_MULT_MASK | PDB_SC_CONT_MASK);

	mask |= PDB_SC_LDMOD(conf->load_value_mode);
	mask |= PDB_SC_PRESCALER(conf->prescaler_div);
	mask |= PDB_SC_TRGSEL(conf->trigger_input_source);
	mask |= PDB_SC_MULT(conf->divider_mult_factor);
	if(conf->continuous_mode)
		mask |= PDB_SC_CONT_MASK;

	PDB0->SC = mask;

	setModulusValue(conf->modulus_value);
	setCounterDelayValue(conf->delay_value);

	PDB0->SC |= PDB_SC_LDOK_MASK;
}

void PDBchangeFrequency(uint32_t value){
	setModulusValue((uint32_t)(BUS_CLOCK/value-1));
	setCounterDelayValue((uint32_t)(BUS_CLOCK/value-1));
}


void setModulusValue(uint32_t value){
	PDB0->MOD = PDB_MOD_MOD(value);
}

void setCounterDelayValue(uint32_t value){
	PDB0->IDLY = PDB_IDLY_IDLY(value);
}

void clockGating(void){
	SIM->SCGC6 |= SIM_SCGC6_PDB_MASK;
}

void getPDBdefaultConfig(pdb_config_t* conf){
	conf->load_value_mode = PDB_LOAD_VALUE_DEF;
	conf->prescaler_div = PDB_PRESCALER_DEF;
	conf->divider_mult_factor = PDB_DIV_MULT_DEF;
	conf->trigger_input_source = PDB_TRIG_INPUT_DEF;//software trigger
	conf->continuous_mode = true;
	conf->modulus_value = PDB_MOD_VALUE_DEF;
	conf->delay_value = PDB_DELAY_VALUE_DEF;
}
void getPDBforDACdefaultConfig(pdb_dac_config_t* conf){
	conf->channel = PDB_DAC_CH_DEF;
	conf->external_trig_input = false;
	conf->interval_trigger = true;
	conf->interval_value = PDB_DAC_INTERVAL_VALUE_DEF;
}

void initPDBdac(pdb_dac_config_t* conf){
	uint32_t mask = 0U;
	if(conf->external_trig_input)
		mask |= PDB_INTC_EXT_MASK;
	if(conf->interval_trigger)
		mask |= PDB_INTC_TOE_MASK;
	PDB0->DAC[conf->channel].INTC = mask;
	PDB0->DAC[conf->channel].INT = PDB_INT_INT(conf->interval_value);
}

void PDBsoftwareTrigger(void){
	PDB0->SC |= PDB_SC_LDOK_MASK;
	PDB0->SC |= PDB_SC_SWTRIG_MASK;
}
