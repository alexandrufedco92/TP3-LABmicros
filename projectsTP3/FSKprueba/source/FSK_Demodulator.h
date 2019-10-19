/*
 * FSK_Demodulator.h
 *
 * Module that  deals with the demodulation of FSK signals
 *  Created on: Oct 18, 2019
 *      Author: G5
 */
#include <stdint.h>

#ifndef FSK_DEMODULATOR_H_
#define FSK_DEMODULATOR_H_

void DemodulatorInit(uint8_t buffer_size);
DemodulateSignal(int16_t* recieved, int16_t* result);




#endif /* FSK_DEMODULATOR_H_ */
