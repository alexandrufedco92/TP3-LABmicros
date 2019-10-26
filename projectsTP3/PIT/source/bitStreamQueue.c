/*
 * bitStreamQueue.c
 *
 *  Created on: Oct 26, 2019
 *      Author: Manuel Mollon
 */

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <bitStreamQueue.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_TOP	(QUEUE_SIZE-2)
#define QUEUE_SIZE	100
/* Index for most significant bit in char. */
#define MSB_IDX	7
/* Index for least significant bit in char. */
#define LSB_IDX	0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct{
	char bitStreamQueue[QUEUE_SIZE];
	int top;
	uint8_t bitIdx;
}queue_t;

static queue_t queue;

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/

void bitStreamQueueInit(void){
	queue.top = -1;
	queue.bitIdx = LSB_IDX;
}

bool pushChar(char value){
	if(queue.top == MAX_TOP){ /* Queue full. */
		return false;
	}
	else{ /* Insert new item in queue. */
		queue.top++;
		queue.bitStreamQueue[queue.top] = value;
	}
}

bool isQueueEmpty(void){
	if(queue.top == -1){ /* Queue Empty.*/
		return true;
	}
	else{
		return false;
	}
}

bool popBit(void){
	bool finalValue;
	/* Current value. */
	char currValue = queue.bitStreamQueue[top];
	/* Mask to detect current bit*/
	char mask = 0x1 <<  queue.bitIdx;
	/* Value to determine current bit value. Results in zeros in other bits and the value of the bit in current bit. */
	maskValue = currValue & mask;
	/* Save bit Value*/
	if(maskValue == 0){
		finalValue = false; /* Current bit is 0. */
	}
	else{
		finalValue = true; /* Current bit is 1. */
	}
	/* Update index in queue. */
	if(queue.bitIdx == MSB_IDX){ /* All bits in char were read. Pop char from queue. */
		queue.top--;
		queue.bitIdx = LSB_IDX;
	}
	else{ /* Other bits still to be read from char. */
		queue.bitIdx++;
	}
	return finalValue;
}




