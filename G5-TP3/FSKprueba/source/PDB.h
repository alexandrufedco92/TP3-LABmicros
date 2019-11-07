/*
 * PDB.h
 *
 *  Created on: Oct 30, 2019
 *      Author: Lu
 */

#ifndef PDB_H_
#define PDB_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
	uint8_t load_value_mode;     
	uint8_t prescaler_div;       
	uint8_t divider_mult_factor; 
	uint8_t trigger_input_source;
	bool continuous_mode;
	uint32_t modulus_value;
	uint32_t delay_value;
}pdb_config_t;

typedef struct{
	uint8_t channel;
	bool external_trig_input;
	bool interval_trigger;
	uint32_t interval_value;
}pdb_dac_config_t;
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


//estructura para inicializar el PDB
void getPDBdefaultConfig(pdb_config_t*);

//inicializa PDB con la estructura anterior
void initPDB(pdb_config_t*);

//estructura para inicializar el PDB para triggerear DAC
void getPDBforDACdefaultConfig(pdb_dac_config_t* conf);

//inicializa los registros del PDB para triggerear DAC
void initPDBdac(pdb_dac_config_t*);

//realiza un software trigger
void PDBsoftwareTrigger(void);

//cambia frecuencia de trigger del PDB
void PDBchangeFrequency(uint32_t frequency);

#endif /* PDB_H_ */
