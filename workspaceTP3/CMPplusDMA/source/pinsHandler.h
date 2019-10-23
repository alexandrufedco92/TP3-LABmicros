/*
 * pinsHandler.h
 *
 *  Created on: 18 sep. 2019
 *      Author: G5
 */

#ifndef PINSHANDLER_H_
#define PINSHANDLER_H_

#include "MK64F12.h"

#include <stdint.h>
#include <stdbool.h>



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Ports
enum { PA, PB, PC, PD, PE };

// Convert port and number into pin ID
// Ex: PTB5  -> PORTNUM2PIN(PB,5)  -> 0x25
//     PTC22 -> PORTNUM2PIN(PC,22) -> 0x56
#define PORTNUM2PIN(p,n)    (((p)<<5) + (n))
#define PIN2PORT(p)         (((p)>>5) & 0x07)
#define PIN2NUM(p)          ((p) & 0x1F)

//Caracteristicas de los puertos
#ifndef NUMBER_OF_PORTS
#define NUMBER_OF_PORTS 5
#define NUMBER_OF_PINS_PORT 32
#endif //NUMBER_OF_PORTS

// IRQ modes
enum {
    IRQ_MODE_DISABLE = 0,
    IRQ_MODE_RISING_EDGE = 9,
    IRQ_MODE_FALLING_EDGE = 10,
    IRQ_MODE_BOTH_EDGES = 11,

    IRQ_CANT_MODES = 4
};
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint8_t pin_t;

typedef void (*pinIrqFun_t)(void);


_Bool isPinValid(pin_t pin);

void setPCRmux(PORT_Type * p2port, uint8_t numPin, uint8_t mux);

void setPCRpullEnable(PORT_Type * portPointer, uint8_t numPin);

void setPCRpullUp(PORT_Type * portPointer, uint8_t numPin);

void setPCRpullDown(PORT_Type * portPointer, uint8_t numPin);

void setPCRirqc(PORT_Type * p2port, uint8_t numPin, uint8_t irqMode);

void setPCRopenDrainEnable(PORT_Type * portPointer, uint8_t numPin);



#endif /* PINSHANDLER_H_ */
