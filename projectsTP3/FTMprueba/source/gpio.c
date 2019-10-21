/*
 * gpio.c
 *
 *  Created on: 23 ago. 2019
 *      Author: g5
 */

#include "gpio.h"

//#include "core_cm4.h"

//Caracteristicas de los puertos
#ifndef NUMBER_OF_PORTS
#define NUMBER_OF_PORTS 5
#define NUMBER_OF_PINS_PORT 32
#endif //NUMBER_OF_PORTS

// Mux consts
#ifndef GPIO_MUX
#define GPIO_MUX     001
#endif // GPIO_MUX

//Struct Types
#ifndef GPIO_STRUCT
#define GPIO_STRUCT 1
#define PORT_STRUCT 2
#endif //GPIO_STRUCT

//#define NO_IRQ_PIN -1

//Globals
pinIrqFun_t p2irqFunctions[NUMBER_OF_PORTS][NUMBER_OF_PINS_PORT];
//uint8_t pinsIRQenable[NUMBER_OF_PORTS][NUMBERS_OF_PINS_PORT];
//_Bool arraysInitialize = false;

//void initializeGPIOirq(void);

_Bool isPinValid(pin_t pin);



void setPCRpullEnable(PORT_Type * portPointer, uint8_t numPin);

void setPCRpullUp(PORT_Type * portPointer, uint8_t numPin);

void setPCRpullDown(PORT_Type * portPointer, uint8_t numPin);

void setGPIOdataOut(GPIO_Type * gpioPortPointer, uint8_t numPin, _Bool value);



void setGPIOddr(GPIO_Type * p2port, uint8_t numPin, uint32_t mode);

void gpioMode (pin_t pin, uint8_t mode);

void gpioWrite (pin_t pin, _Bool value);

void gpioToggle (pin_t pin);


_Bool gpioRead (pin_t pin);

void clockGating(uint8_t port, uint8_t numPin);

//void add2PinsIRQenable(uint8_t port, uint8_t numPin);

//void removeFromPinsIRQenable(uint8_t port, uint8_t numPin);

_Bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun);

void PORTX_IRQHandler(uint8_t port);
//Sobrescritura de Handler para la interrupción generada por el puerto A.
void PORTA_IRQHandler(void);

void PORTB_IRQHandler(void);
void PORTC_IRQHandler(void);
void PORTD_IRQHandler(void);
void PORTE_IRQHandler(void);


_Bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun)
{
	_Bool succeed = false;
	uint8_t port;
	uint8_t numPin;
	IRQn_Type p2portsIRQ[] = PORT_IRQS;
	PORT_Type * portPointer[] = PORT_BASE_PTRS;
	if(isPinValid(pin))
	{

		port = PIN2PORT(pin);
		numPin = PIN2NUM(pin);


		if (irqMode != GPIO_IRQ_CANT_MODES)
		{
			setPCRirqc(portPointer[port], numPin, irqMode);
			p2irqFunctions[port][numPin] = irqFun;

			succeed = true;
		}
		//NVIC_EnableIRQ(p2portsIRQ[port]);
		NVIC_EnableIRQ(p2portsIRQ[port]);
	}

	return succeed;
}

void PORTX_IRQHandler(uint8_t port)
{
	_Bool irqFounded = false;
	int i;
	uint32_t actualPCR; //variable auxiliar
	PORT_Type * portPointer[] = PORT_BASE_PTRS;
	for(i = 0; (i < NUMBER_OF_PINS_PORT) && (!irqFounded); i++)
	{
		if( ((portPointer[port])->PCR[i]) & PORT_PCR_ISF_MASK )  //si i es el numero de pin a atender, procedo a atenderlo
		{
			irqFounded = true;
			p2irqFunctions[port][i]();   //Realizo la rutina preparada por el programador
			//a continuación escribo "1 to clear" (w1c).
			actualPCR = portPointer[port]->PCR[i]; //obtengo el antiguo PCR para luego actualizarlo
			actualPCR = actualPCR & (~PORT_PCR_ISF_MASK);
			portPointer[port]->PCR[i] = actualPCR | PORT_PCR_ISF(HIGH); //write 1 (HIGH) to clear.
		}
	}
}
void PORTA_IRQHandler(void)
{
	PORTX_IRQHandler(PA);
}
void PORTB_IRQHandler(void)
{
	PORTX_IRQHandler(PB);
}
void PORTC_IRQHandler(void)
{
	PORTX_IRQHandler(PC);
}
void PORTD_IRQHandler(void)
{
	PORTX_IRQHandler(PD);
}
void PORTE_IRQHandler(void)
{
	PORTX_IRQHandler(PE);
}




/**
* @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void gpioMode (pin_t pin, uint8_t mode)
{
	uint8_t port = PIN2PORT(pin);
	uint8_t numPin = PIN2NUM(pin);
	PORT_Type * portPointer[] = PORT_BASE_PTRS;
	GPIO_Type * gpioPortPointer[] = GPIO_BASE_PTRS;
	if (isPinValid(pin))  //procedo a configurar el pin siempre que este pertenezca a algunos de los puertos A,B,C,D o E
	{
		//clockGating(port, numPin);

		setPCRmux(portPointer[port], numPin, GPIO_MUX); //configuro el pin como GPIO modificando el mux del PCR

		if(mode == INPUT_PULLUP || mode == INPUT_PULLDOWN) //se modifica el PCR en caso de que el usuario requiera un pull up/down por software
		{
			setPCRpullEnable(portPointer[port], numPin);
			if (mode == INPUT_PULLUP)
			{
				setPCRpullUp(portPointer[port], numPin);
			}
			else
			{
				setPCRpullDown(portPointer[port], numPin);
			}
			mode = INPUT;

		}
		setGPIOddr(gpioPortPointer[port], numPin, (uint32_t) mode); //configuro el pin como entrada o salida modificando el data direction register (ddr)
		//clockGating(port, numPin);
	}
}


void clockGating(uint8_t port, uint8_t numPin)
{

	SIM_Type * sim = SIM;
	switch(port)
	{
		case PA:
			sim->SCGC5 |= SIM_SCGC5_PORTA(numPin<<1);
			break;
		case PB:
			sim->SCGC5 |= SIM_SCGC5_PORTB(numPin<<1);
			break;
		case PC:
			sim->SCGC5 |= SIM_SCGC5_PORTC(numPin<<1);
			break;
		case PD:
			sim->SCGC5 |= SIM_SCGC5_PORTD(numPin<<1);
			break;
		case PE:
			sim->SCGC5 |= SIM_SCGC5_PORTE(numPin<<1);
			break;
	}
}


/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, _Bool value)
{
	uint8_t port = PIN2PORT(pin);
	uint8_t numPin = PIN2NUM(pin);
	GPIO_Type * gpioPortPointer[] = GPIO_BASE_PTRS;
	uint32_t maskPin = (uint32_t)(1 << numPin);
	//gpioPortPointer = (GPIO_Type *)getStructAccess(GPIO_STRUCT, port);
	if((gpioPortPointer[port]->PDDR) & maskPin)  //Si es un pin de output
	{
		setGPIOdataOut(gpioPortPointer[port], numPin, value);

	}
}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin)
{
	uint8_t port = PIN2PORT(pin);
	uint8_t numPin = PIN2NUM(pin);
	GPIO_Type * gpioPortPointer[] = GPIO_BASE_PTRS;
	//gpioPortPointer = (GPIO_Type *)getStructAccess(GPIO_STRUCT, port);
	uint32_t mask2write = ((uint32_t)(1<<numPin));
	uint32_t mask2delete = ~mask2write;
	gpioPortPointer[port]->PTOR = gpioPortPointer[port]->PTOR & mask2delete;
	gpioPortPointer[port]->PTOR = gpioPortPointer[port]->PTOR | mask2write;
}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
_Bool gpioRead (pin_t pin)
{
	uint8_t port = PIN2PORT(pin);
	uint8_t numPin = PIN2NUM(pin);
	GPIO_Type * gpioPortPointer[] = GPIO_BASE_PTRS;
	//gpioPortPointer = (GPIO_Type *)getStructAccess(GPIO_STRUCT, port);
	uint32_t mask2read = (uint32_t)(1<<numPin);
	return ((gpioPortPointer[port]->PDIR) & mask2read);

}


_Bool isPinValid(pin_t pin)
{
	_Bool ret = false;
	if((pin >= PORTNUM2PIN(PA,0)) && (pin <= PORTNUM2PIN(PE,31)))
	{
		ret = true;
	}

	return ret;
}

void setPCRmux(PORT_Type * p2port, uint8_t numPin, uint8_t mux)
{
	uint32_t actualPCR = (p2port->PCR)[numPin]; //obtengo el antiguo PCR para luego actualizarlo
	actualPCR = (actualPCR) & (~PORT_PCR_MUX_MASK); //elimino el contenido anterior del mux del PCR
	actualPCR = actualPCR | PORT_PCR_MUX(mux); //ingreso el mux con el contenido deseado por el usuario de esta función
	(p2port->PCR)[numPin] = actualPCR; //finalmente actualizo el PCR

}

void setPassiveFilter(pin_t pin)
{
	uint8_t port = PIN2PORT(pin);
	uint8_t numPin = PIN2NUM(pin);
	PORT_Type * portPointer[] = PORT_BASE_PTRS;
	uint32_t maskPFE = (HIGH << PORT_PCR_PFE_SHIFT);
	(portPointer[port]->PCR)[numPin] = ((portPointer[port]->PCR)[numPin] | maskPFE);
}

void setPCRpullEnable(PORT_Type * portPointer, uint8_t numPin)
{
	uint32_t maskPE = (HIGH << PORT_PCR_PE_SHIFT);
	(portPointer->PCR)[numPin] = ((portPointer->PCR)[numPin] | maskPE);
}
void setPCRpullUp(PORT_Type * portPointer, uint8_t numPin)
{
	uint32_t maskPE = (HIGH << PORT_PCR_PS_SHIFT);
	(portPointer->PCR)[numPin] = ((portPointer->PCR)[numPin] | maskPE);
}
void setPCRpullDown(PORT_Type * portPointer, uint8_t numPin)
{
	uint32_t maskPE = (HIGH << PORT_PCR_PE_SHIFT);
	(portPointer->PCR)[numPin] = ((portPointer->PCR)[numPin] & (~maskPE));
}


void setGPIOddr(GPIO_Type * p2port, uint8_t numPin, uint32_t mode)
{
	uint32_t maskDDR = (mode << numPin);  //mode es 1 o 0 dependiendo si es INPUT o OUTPUT
	p2port->PDDR = ((p2port->PDDR) | maskDDR);

}

void setGPIOdataOut(GPIO_Type * gpioPortPointer, uint8_t numPin, _Bool value)
{
	uint32_t maskDataOut = (uint32_t)(value << numPin);
	uint32_t mask2delete = ~((uint32_t)(1 << numPin));
	gpioPortPointer->PDOR = (gpioPortPointer->PDOR & mask2delete);
	gpioPortPointer->PDOR = (gpioPortPointer->PDOR | maskDataOut);
}

void setPCRirqc(PORT_Type * p2port, uint8_t numPin, uint8_t irqMode)
{
	uint32_t actualPCR = (p2port->PCR)[numPin]; //obtengo el antiguo PCR para luego actualizarlo
	actualPCR = actualPCR & (~PORT_PCR_IRQC_MASK);
	actualPCR = actualPCR | PORT_PCR_IRQC(irqMode);
	(p2port->PCR)[numPin] = actualPCR; //finalmente actualizo el PCR
}

/*
 *
 * void initializeGPIOirq(void)
{
	int i, j;
	for(i = 0; i < NUMBER_OF_PORTS; i++)
	{
		for(j = 0; j < NUMBER_OF_PINS_PORT; j++)
		{
			pinsIRQenable[i][j] = NO_IRQ_PIN;  //setting global - se comienza sin ningun pin con irq asignado
		}
	}

	arraysInitialize = true; //setting global

}
 * void add2PinsIRQenable(uint8_t port, uint8_t numPin)
{
	int i;
	_Bool pinAdded = false;
	for(i = 0; (i < NUMBER_OF_PINS_PORT) && (!pinAdded); i++)
	{
		if(pinsIRQenable[port][i] == NO_IRQ_PIN)
		{
			pinsIRQenable[port][i] = numPin;
			pinAdded = true;
		}
	}

}
void removeFromPinsIRQenable(uint8_t port, uint8_t numPin)
{
	int i, j;
	_Bool pinRemoved = false;
	for(i = 0; (i < NUMBER_OF_PINS_PORT) && (!pinRemoved); i++)
	{
		if(pinsIQRenable[port][i] == numPin)
		{


			for(j = i; (j < (NUMBER_OF_PINS_PORT-1)) && (pinsIRQenable[port][j] != NO_IRQ_PIN); j++)
			{
				pinsIRQenable[port][j] = pinsIRQenable[port][j+1];   //Shifteo de los pines habilitados para IRQ a la izquierda

			}

			pinsIRQenable[port][j] = NO_IRQ_PIN; //afirmo que el último pin sea NO_IRQ_PIN (no habilitado)
			pinRemoved = true;
		}
	}

}
 *
 *
 */



