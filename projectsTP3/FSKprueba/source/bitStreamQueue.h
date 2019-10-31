/*******************************************************************************
  @file     bitStreamQueue.h
  @brief    char-in/bit-out queue
  @author   Manuel Mollon
 ******************************************************************************/

#ifndef BITSTREAMQUEUE_H_
#define BITSTREAMQUEUE_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
 * DEFINES AND MACROS
 ******************************************************************************/

//Number of bits per frame
#define FRAME_SIZE 11

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Init Queue
 */
void bitStreamQueueInit(void);

/**
 * @brief Pushes char into Queue if there is space.
 * @param value char to be pushed.
 * @return True if push was succesfull. False if char was not pushed (queue full).
 */
bool pushChar(char value);

/**
 * @brief Checks if queue is empty.
 * @return True if Queue is empty, false if there is still space.
 */
bool isQueueEmpty(void);

/**
 * @brief Pop one bit from the queue.
 * @return True if bit is 1, false if bit is 0. Always check if queue is not empty before popping.
 */
bool popBit(void);

bool PushBit(char digital_symbol);

bool IsFrameReady();

char* GetFrame(void);

#endif /* BITSTREAMQUEUE_H_ */
