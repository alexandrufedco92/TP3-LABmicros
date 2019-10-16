/*
 * uart.h
 *
 *  Created on: 18 sep. 2019
 *      Author: G5
 */

#ifndef _UART_H_
#define _UART_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_CANT_IDS 5
#define NO_PARITY_UART 0
#define EVEN_PARITY_UART 1
#define ODD_PARITY_UART 2
#define MAX_BAUD_RATE_UART 11200
#define MIN_BAUD_RATE_UART 9600
#define IS_VALID_BAUD_RATE(x) ( (x >= MIN_BAUD_RATE_UART) && (x <= MAX_BAUD_RATE_UART) )

#define NBITS_8 0
#define NBITS_9 1

#define MAX_MSG_LEN 8

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum {U0, U1, U2, U3, U4};
enum {BLOCKING, NON_BLOCKING_SIMPLE, NON_BLOCKING_FIFO};
typedef struct {
    uint32_t baudRate;
    uint8_t mode;
    uint8_t parity;
    uint8_t nBits;
    int txWaterMark;
    int rxWaterMark;
} uart_cfg_t;



/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize UART driver
 * @param id UART's number
 * @param config UART's configuration (baudrate, parity, etc.)
*/
void uartInit (uint8_t id, uart_cfg_t config);

/**
 * @brief Check if a new byte was received
 * @param id UART's number
 * @return A new byte has being received
*/
uint8_t uartIsRxMsg(uint8_t id);

/**
 * @brief Check how many bytes were received
 * @param id UART's number
 * @return Quantity of received bytes
*/
uint8_t uartGetRxMsgLength(uint8_t id);

/**
 * @brief Read a received message. Non-Blocking
 * @param id UART's number
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id UART's number
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant);

/**
 * @brief Check if all bytes were transfered
 * @param id UART's number
 * @return All bytes were transfered
*/
uint8_t uartIsTxMsgComplete(uint8_t id);

//with resetRXbuffer, the user can ignore all the message that have been arrived to the moment. It has sense only in NON_BLOCKING mode
void resetRXbuffer(uint8_t id);

//isRXrejected returns how many RX chars have been rejected (since the last successful RX) because of lack of memory buffer, then the user would know
//how many chars have to be request to retransmit. If any RX has been rejected, the function returns 0. It has sense only in NON_BLOCKING mode.

uint8_t isRXrejected(uint8_t id);


/*******************************************************************************
 ******************************************************************************/

#endif // _UART_H_
