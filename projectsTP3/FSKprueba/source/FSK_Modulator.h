/*
 * FSK_Modulator.h
 *
 *  Created on: Oct 30, 2019
 *      Author: G5
 */

#ifndef FSK_MODULATOR_H_
#define FSK_MODULATOR_H_
#define DAC_VERSION			/*	Uses modulator with DAC if defined, if commented the modulator
								is implemented via alternate method without DAC. */

/******************************************************************
 * 						INCLUDED HEADERS
 ******************************************************************/

/******************************************************************
 * 						HEADER FUNCTIONS
 *******************************************************************/

/**
 * @brief Initialize Modulator.
 */
void ModulatorInit(void);

#endif /* FSK_MODULATOR_H_ */
