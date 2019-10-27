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

#ifndef FSK_DEMODULATOR_H_
#define FSK_DEMODULATOR_H_

/**
 * @brief Initializes resources needed for FSK demodulation
 * @param fs Sample frequency of the FSK signal
 * @param buffer_size Number of elements to demodulate
*/
void DemodulatorInit(uint32_t fs);

/**
 * @brief Demodulates a FSK signal and stores values
 * @param recieved Buffer with the data to demodulate
 * @param buffer_size Number of elements to demodulate
*/
void DemodulateSignal(float* recieved,uint8_t buffer_size);

/**
 * @brief Gets the values of the last demodulated signal
 * @param data_buffer Buffer in which to paste the desired values
 * @param num_samples Number of elements to paste on buffer
*/
void GetData(float* data_buffer,uint8_t num_samples);




#endif /* FSK_DEMODULATOR_H_ */
