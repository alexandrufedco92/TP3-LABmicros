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
	uint8_t load_value_mode;                             /*!< Select the load value mode. */
	uint8_t prescaler_div;                        /*!< Select the prescaler divider. */
	uint8_t divider_mult_factor; /*!< Multiplication factor select for prescaler. */
	uint8_t trigger_input_source;                   /*!< Select the trigger input source. */
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



void getPDBdefaultConfig(pdb_config_t*);

void initPDB(pdb_config_t*);

void getPDBforDACdefaultConfig(pdb_dac_config_t* conf);

void initPDBdac(pdb_dac_config_t*);

void PDBsoftwareTrigger(void);

#endif /* PDB_H_ */
