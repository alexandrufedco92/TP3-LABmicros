/*
 * DAC.h
 *
 *  Created on: 17 oct. 2019
 *      Author: malen
 */

#ifndef DAC_H_
#define DAC_H_

#define DAC_REFERENCE_1 0
#define DAC_REFERENCE_2 1
#define DAC_REFERENCE_SELECTED DAC_REFERENCE_1 //The maximum voltage of the DAC's output
												//is equal to the reference voltage.
												//The minimum voltage of the DAC's output
												//is equal to the reference voltage divided by 4096

typedef enum {DAC0_ID, DAC1_ID, NUMBER_OF_DACS}DACids;

typedef struct
{
	//aca se puede configurar si es necesario buffer, interrupciones, etc.

}DACconfig_t;

void DACinit(DACids id, DACconfig_t * config);

void writeDACvalue(DACids id, float value);


#endif /* DAC_H_ */
