/*
 * FSK_Demodulator.h
 *
 * Module that  deals with the demodulation of FSK signals. Sine waves
 * with a frequency of 1200Hz are associated with a binary '1',while
 * a frequency of 2200Hz is associated with a binary '0'.
 *  Created on: Oct 18, 2019
 *      Author: G5
 */
#include <stdint.h>
#include <stdbool.h>

#ifndef FSK_DEMODULATOR_H_
#define FSK_DEMODULATOR_H_

/**
 * @brief Initializes resources needed for FSK demodulation
 * @param fs Sample frequency of the FSK signal
 * @param buffer_size Number of elements to demodulate
*/
void DemodulatorInit(void);

/**
 * @brief Demodulates a FSK signal and stores values
 * @param recieved Buffer with the data to demodulate
 * @param buffer_size Number of elements to demodulate
 * @param result Buffer in which to paste the digital signal values.
 * @return logical symbol of signal or -1 if more samples are needed to finish.
*/
bool DemodulateSignal(float recieved);

bool IsDemodulationFinished(void);


#endif /* FSK_DEMODULATOR_H_ */
