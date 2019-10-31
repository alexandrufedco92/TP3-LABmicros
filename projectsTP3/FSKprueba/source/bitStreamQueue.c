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
static char signal_frame[FRAME_SIZE];
static bool frame_ready;
static int frame_index =0;

static void recursiveStore(int index, char value);

static void recursiveStore(int index, char value){
	if(index == -1){
		queue.bitStreamQueue[index+1] = value;
	}
	else{
		queue.bitStreamQueue[index+1] = queue.bitStreamQueue[index];
		index--;
		recursiveStore(index,value);
	}
}

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/

void bitStreamQueueInit(void){
	queue.top = -1;
	queue.bitIdx = MSB_IDX;
}

bool pushChar(char value){
	if(queue.top == MAX_TOP){ /* Queue full. */
		return false;
	}
	else{ /* Insert new item in queue. */
		recursiveStore(queue.top, value);
		queue.top++;
		return true;
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
	char currValue = queue.bitStreamQueue[queue.top];
	/* Mask to detect current bit*/
	char mask = 0x1 <<  queue.bitIdx;
	/* Value to determine current bit value. Results in zeros in other bits and the value of the bit in current bit. */
	char maskValue = currValue & mask;
	/* Save bit Value*/
	if(maskValue == 0){
		finalValue = false; /* Current bit is 0. */
	}
	else{
		finalValue = true; /* Current bit is 1. */
	}
	/* Update index in queue. */
	if(queue.bitIdx == LSB_IDX){ /* All bits in char were read. Pop char from queue. */
		queue.top--;
		queue.bitIdx = MSB_IDX;
	}
	else{ /* Other bits still to be read from char. */
		queue.bitIdx--;
	}
	return finalValue;
}

bool PushBit(char bit)
{
	if(frame_ready)
	{
		return false; //Frame is full cannot add more bits
	}
	else
	{
		signal_frame[frame_index] = bit;
		frame_index++;

		if( frame_index == FRAME_SIZE)
			frame_ready = true; //Frame is completed
		return true;
	}
}

bool IsFrameReady(void)
{
	return frame_ready;
}

char* GetFrame(void)
{
	return signal_frame;
}

bool pushString(char * string, uint8_t cant){
	bool success = false;
	for(uint_8 i = 0;i<cant;i++){
		success = pushChar(string[i]);
		if(!success){ /* Failed to push. */
			return false;
		}
	}
	return true;
}
