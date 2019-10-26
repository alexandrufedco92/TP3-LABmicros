/*
 * DAC.h
 *
 *  Created on: 17 oct. 2019
 *      Author: malen
 */

#ifndef DAC_H_
#define DAC_H_

#define VIN_SELECTED 3.3
#define DAC_REFERENCE_1 0
#define DAC_REFERENCE_2 1
#define DAC_REFERENCE_SELECTED DAC_REFERENCE_1 //The maximum voltage of the DAC's output
												//is equal to the reference voltage.
												//The minimum voltage of the DAC's output
												//is equal to the reference voltage divided by 4096

typedef enum {DAC0_ID, DAC1_ID, NUMBER_OF_DACS}DACids;
typedef enum {DAC_NORMAL_MODE, DAC_BUFFER_MODE}DACmode;
typedef enum {DAC_DMA_DISABLE, DAC_DMA_ENABLE}DACdmaMode;
typedef enum {DAC_TRIGGER_HW, DAC_TRIGGER_SW}DACtriggerMode;
typedef enum {DAC_WM_RP_EV, DAC_RESET_RP_EV}DACev;

typedef void (*DAC_callback_t)(DACev);

#define DAC_BUFFER_SIZE (16)

typedef struct
{
	DACmode mode;
	DACdmaMode dmaMode;
	DACtriggerMode triggerMode;
	DAC_callback_t p2callback;

}DACconfig_t;

void DACinit(DACids id, DACconfig_t * config);

void writeDACvalue(DACids id, int bufferIndex, float value);

void writeDACdata(DACids id, int bufferIndex, int data);

int shapeValue2DACdata(float value);

void updateSoftwareTrigger(DACids id);

int DACgetBFreadPointer(DACids id);
void DACincBFreadPointer(DACids id);


#endif /* DAC_H_ */
