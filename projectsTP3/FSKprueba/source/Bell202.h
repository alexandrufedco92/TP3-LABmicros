/*
 * Bell202.h
 *
 * Module that simulates a Bell 202 FSK Modem
 *
 *  Created on: Oct 30, 2019
 *      Author: G5
 */



#ifndef BELL202_H_
#define BELL202_H_
typedef enum{DSP_VERSION, PWM_VERSION}modem_version_t;

void ModemInit( modem_version_t version);
void ModemRun(void);

#endif /* BELL202_H_ */
