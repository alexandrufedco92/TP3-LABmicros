/*
 * measureFreq.h
 *
 *  Created on: 25 oct. 2019
 *      Author: malen
 */

#ifndef MEASUREFREQ_H_
#define MEASUREFREQ_H_

typedef struct{
	int freq; //Hz
	_Bool freqChanged;
}measureFreq_t;

void initFreqMeasure(measureFreq_t * measureDataBase);

#endif /* MEASUREFREQ_H_ */
