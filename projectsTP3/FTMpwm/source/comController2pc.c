/*
 * comController2pc.c
 *
 *  Created on: 4 oct. 2019
 *      Author: G5
 */

#include "comController2pc.h"
#include "timer.h"
#include "uart.h"
#include <stdbool.h>

#define MAX_PACKAGE_PC_LEN 6

#define ANSWER_MSG_LEN 1

#define ACK_ANSWER 'A'
#define ERROR_ANSWER 'E'

int cantCharsInPackage;
char package2pc[MAX_PACKAGE_PC_LEN];
char answerMsg[ANSWER_MSG_LEN];
int errorsCounter = 0;

void rxCallback(void);
_Bool areStringsEqual(char * answerMsg, char * ackMsg, int cant);

void initResourcesController2pc(void)
{
	errorsCounter = 0;
	cantCharsInPackage = 0;

	uart_cfg_t config;
	config.baudRate = 15500;
	config.nBits = 8;
	config.parity = NO_PARITY_UART;
	config.rxWaterMark = 5;
	config.txWaterMark = 2;
	config.mode = NON_BLOCKING_SIMPLE;

	uartInit (U0, config);
	InitializeTimers(); //timers are necessary to take into account the time that the computer takes to answer
}

void sendMessage2pc(char idBoard, char * p2coord, char coordName, int cant)
{
	int i;
	//generation of the package
	package2pc[0] = idBoard;
	package2pc[1] = coordName;
	for(i = 0; i < cant; i++)
	{
		package2pc[i + 2] = p2coord[i];
	}
	cantCharsInPackage = cant + 2;
	//start communication
	while(!uartWriteMsg(U0, package2pc, cant + 2));
	 //cant + 1 (coordName) + 1 (idBoard) = cant + 2
	//SetTimer(TIME_OUT_PC_ANSWER, MAX_TIME_REACTION, rxCallback);
}

void rxCallback(void)
{
	char ackDesired = ACK_ANSWER;
	if(!(uartIsRxMsg(U0) && (uartGetRxMsgLength(U0) == ANSWER_MSG_LEN) && (uartReadMsg(U0, answerMsg, ANSWER_MSG_LEN) == ANSWER_MSG_LEN) && areStringsEqual(answerMsg, &ackDesired, ANSWER_MSG_LEN)))
	{
		//if enter in this branch, is consider an error, so it necessary to send message again
		if(errorsCounter < MAX_ERRORS_ALLOWED)
		{
			uartWriteMsg(U0, package2pc, cantCharsInPackage); //cant + 1 (coordName) + 1 (idBoard) = cant + 2
			//RestartTimer(TIME_OUT_PC_ANSWER);
		}
	}
	else
	{
		//DisableTimer(TIME_OUT_PC_ANSWER);
	}
}

_Bool areStringsEqual(char * answerMsg, char * ackMsg, int cant)
{
	_Bool differenceFounded = false;
	int i;
	for(i = 0; (i < cant) && (!differenceFounded); i++)
	{
		if(answerMsg[i] != ackMsg[i])
		{
			differenceFounded = true;
		}
	}
	return !differenceFounded;
}


