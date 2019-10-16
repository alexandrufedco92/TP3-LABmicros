/*
 * uart.c
 *
 *  Created on: 18 sep. 2019
 *      Author: G5
 */

#include "uart.h"
#include "pinsHandler.h"
#include <stdbool.h>
#include "hardware.h"



#define MAX_BAUD_RATE 0x1FFF
#define MIN_BAUD_RATE 0x0000
#define BAUD_RATE_DEFAULT 15500

#define MAX_WATER_MARK 128
#define TX_WATER_MARK_DEFAULT 2
#define RX_WATER_MARK_DEFAULT 5
#define NBIT_DEFAULT 8

#define DISABLE_MODE 0 //disable port interrupts for uart's pins

#define UART_MUX 3

#define TX_MESSAGE_MAX_LEN 8

#define RX_BUFFER_LEN 8
#define TX_BUFFER_LEN 200

#define EMPTY -1
#define TX_FIFO_SIZE_K64 7
#define RX_FIFO_SIZE_K64 7

							//RX_0				//RX_1				//RX_2					//RX_3				//RX_4
static uint8_t uartRXpins[] = {PORTNUM2PIN(PB, 16), PORTNUM2PIN(PC, 3), PORTNUM2PIN(PD, 2), PORTNUM2PIN(PC, 16), PORTNUM2PIN(PE, 24)};

							//TX_0				//TX_1				//TX_2				//TX_3				//TX_4
static uint8_t uartTXpins[] = {PORTNUM2PIN(PB, 17), PORTNUM2PIN(PC, 4), PORTNUM2PIN(PD, 3), PORTNUM2PIN(PC, 17), PORTNUM2PIN(PE, 25)};

static uint8_t uartMode[UART_CANT_IDS] = {BLOCKING, BLOCKING, BLOCKING, BLOCKING, BLOCKING};

//variables de RX
static char RXbuffers[UART_CANT_IDS][RX_BUFFER_LEN];
static int markersRXbuffer[UART_CANT_IDS] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
static _Bool flagsReading[UART_CANT_IDS] = {false, false, false, false, false};
static uint8_t countersRXfailed[UART_CANT_IDS] = {0, 0, 0, 0, 0};

//variables de TX
static char TXbuffers[UART_CANT_IDS][TX_BUFFER_LEN];
static uint8_t outMarkersTXbuffer[UART_CANT_IDS] = {0, 0, 0, 0, 0};
static uint8_t inMarkersTXbuffer[UART_CANT_IDS] = {0, 0, 0, 0, 0};
static uint8_t lensTXbuffer[UART_CANT_IDS] = {0, 0, 0, 0, 0};
static _Bool flagsWriting[UART_CANT_IDS] = {false, false, false, false, false};

//
uint8_t uartIRQs_TX_RX[] = UART_RX_TX_IRQS;
uint8_t uartIRQs_ERR[] = UART_ERR_IRQS;
uint8_t uartIRQs_LON[] = UART_LON_IRQS;

void UART_clockGating(uint8_t id);
void UART_setBaudRate(UART_Type * p2uart, uint32_t baudRate);
void UART_setParity(UART_Type * p2uart, uint8_t parity);
void UART_setNdataBits(UART_Type * p2uart, uint8_t nBits);

void UARTX_LON_IRQHandler(uint8_t id);
void UARTX_RX_TX_IRQHandler(uint8_t id);
void UARTX_ERR_IRQHandler(uint8_t id);

void UART0_LON_IRQHandler(void);
void UART0_RX_TX_IRQHandler(void);
void UART0_ERR_IRQHandler(void);
void UART1_RX_TX_IRQHandler(void);
void UART1_ERR_IRQHandler(void);
void UART2_RX_TX_IRQHandler(void);
void UART2_ERR_IRQHandler(void);
void UART3_RX_TX_IRQHandler(void);
void UART3_ERR_IRQHandler(void);
void UART4_RX_TX_IRQHandler(void);
void UART4_ERR_IRQHandler(void);

void copyTXmsg(uint8_t id, const char * msg, uint8_t cant);

void uartInit (uint8_t id, uart_cfg_t config)
{
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	PORT_Type * portPointers[] = PORT_BASE_PTRS;
	uint8_t portRX, portTX, numPinRX, numPinTX;
	//uint8_t fifoSize;

	if ((id >= 0) && (id < UART_CANT_IDS)) //solo se hace algo si el id es válido
	{
		p2uart = p2uartsArray[id];
		UART_clockGating(id);
		p2uart->C2 &= (~(UART_C2_TE_MASK | UART_C2_RE_MASK)); //para configurar correctamente deshabilito la transmision y la recepcion


		//UART_setParity(p2uart, config.parity);
		UART_setBaudRate(p2uart, config.baudRate);
		//UART_setNdataBits(p2uart, config.nBits);

		portRX = PIN2PORT(uartRXpins[id]);
		numPinRX = PIN2NUM(uartRXpins[id]);
		portTX = PIN2PORT(uartTXpins[id]);
		numPinTX = PIN2NUM(uartTXpins[id]);

		setPCRmux(portPointers[portRX], numPinRX, UART_MUX);
		setPCRmux(portPointers[portTX], numPinTX, UART_MUX);
		setPCRirqc(portPointers[portRX], numPinRX, DISABLE_MODE); //deshabilito interrupciones de puerto, para que transmitir o
		setPCRirqc(portPointers[portTX], numPinTX, DISABLE_MODE); //recibir no me ocasione interrupción de puerto

		uartMode[id] = config.mode; //se guarda el modo (BLOCKING, NON_BLOCKING_SIMPLE, FIFO, etc).

		if(config.mode != BLOCKING) //configuración para version no bloqueante
		{
			NVIC_EnableIRQ(uartIRQs_TX_RX[id]);
			NVIC_EnableIRQ(uartIRQs_ERR[id]);
			if(id == U0)
			{
				NVIC_EnableIRQ(uartIRQs_LON[id]);
			}
			if(config.mode == NON_BLOCKING_FIFO)
			{
				//seteo enable de TXFIFO y RXFIFO
				//fifoSize = (p2uart->PFIFO & UART_PFIFO_RXFIFOSIZE_MASK) >> UART_PFIFO_RXFIFOSIZE_SHIFT;
				p2uart->RWFIFO = ((config.rxWaterMark > 0) && (config.rxWaterMark <= MAX_WATER_MARK)? config.rxWaterMark : RX_WATER_MARK_DEFAULT);
				p2uart->PFIFO |= (UART_PFIFO_RXFE_MASK);  //habilito RXFIFO
				p2uart->SFIFO |= UART_SFIFO_RXOF_MASK | UART_SFIFO_RXUF_MASK;
				p2uart->CFIFO |= UART_CFIFO_RXFLUSH_MASK;
				p2uart->SFIFO |= UART_SFIFO_RXUF_MASK;
				p2uart->TWFIFO = ((config.txWaterMark > 0) && (config.txWaterMark <= MAX_WATER_MARK)? config.txWaterMark : TX_WATER_MARK_DEFAULT);
				p2uart->PFIFO |= (UART_PFIFO_TXFE_MASK);  //habilito TXFIFO
				p2uart->SFIFO |= UART_SFIFO_TXOF_MASK;
				p2uart->CFIFO |= UART_CFIFO_TXFLUSH_MASK;

			}

			p2uart->C2 |= UART_C2_RIE_MASK;
		}
		p2uart->C2 |= (UART_C2_RE_MASK);

	}
}

uint8_t uartIsRxMsg(uint8_t id)
{
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	p2uart = p2uartsArray[id];
	uint8_t ret = 0;
	if ((uartMode[id] == NON_BLOCKING_FIFO) || (uartMode[id] == NON_BLOCKING_SIMPLE))
	{
		if(markersRXbuffer[id] == EMPTY)
		{
			ret = 0;
		}
		else
		{
			ret = (uint8_t) markersRXbuffer[id] + 1;
		}
	}
	else if(uartMode[id] == BLOCKING)
	{
		ret = p2uart->S1 & UART_S1_RDRF_MASK;
	}

	return ret;
}

uint8_t uartGetRxMsgLength(uint8_t id)
{
	return (markersRXbuffer[id] + 1);
}

uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant)
{
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	uint8_t ret = 0;
	int i = 0, j;
	if ((id >= 0) && (id < UART_CANT_IDS)) //solo se hace algo si el id es válido
	{
		p2uart = p2uartsArray[id];
		if(uartMode[id] == BLOCKING)  //bloquea hasta leer "cant" numero de chars.
		{
			while((p2uart->S1 & UART_S1_RDRF_MASK) && (i < cant)) //tengo algo que leer?
			{
				msg[i] = p2uart->D;
				i++;
			}
			ret = i;
		}
		else if(((uartMode[id] == NON_BLOCKING_FIFO) || (uartMode[id] == NON_BLOCKING_SIMPLE)) && (markersRXbuffer[id] != EMPTY)) //lee todo lo del buffer, siempre y cuando halla algo para leer
		{
			flagsReading[id] = true; //aviso que voy a leer asi no me pisan el buffer mientras lo leo
			for(i = 0; (i < cant) && (i <= markersRXbuffer[id]); i++)
			{
				msg[i] = RXbuffers[id][i];  //hago pop de la cantidad de char. si no llego a "cant" de chars en el buffer, popeo todo lo que tengo
			}
			//actualizo el buffer (lo shifteo) y el marker si es necesario
			for(j = 0; j < (markersRXbuffer[id] + 1 - cant); j++) //solo entro al for cuando cant es menor a (marker + 1)
			{
				RXbuffers[id][j] = RXbuffers[id][cant + j];   //muevo al principio del buffer los datos que quedaron sin popear
			}
			markersRXbuffer[id] = (j == 0? EMPTY : (j-1));   //corrijo el marker que apunta al último char
			flagsReading[id] = false; //aviso que termine de leer asi escriben el buffer tranquilos
			ret = i;
		}
	}
	return ret;
}

uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant)
{
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	int i = 0;
	uint8_t cantTX = 0;
	_Bool uartWasSleeping = false;
	if ((id >= 0) && (id < UART_CANT_IDS)) //solo se hace algo si el id es válido
	{
		p2uart = p2uartsArray[id];
		if(uartMode[id] == BLOCKING)
		{
			while(i < cant)	 //me queda algo por transmitir?
			{
				if((p2uart->S1 & UART_S1_TDRE_MASK)) //puedo transmitir?
				{
					p2uart->D = msg[i];
					i++;
				}
			}
			cantTX = cant;
		}
		else if((uartMode[id] == NON_BLOCKING_SIMPLE) || (uartMode[id] == NON_BLOCKING_FIFO))
		{
			if(lensTXbuffer[id] == 0) //no habia nada para transmitiendose, recuerdo que debo iniciar la transmision
			{
				uartWasSleeping = true;
			}
			cantTX = TX_BUFFER_LEN - lensTXbuffer[id]; //cantTX hasta aqui es la cantidad disponible
			cantTX = ( (cant <= cantTX)? cant : cantTX ); //cantTX es el minimo entre la cantidad disponible y la cantidad requerida
			if (cantTX == cant)
			{
				copyTXmsg(id, msg, cantTX); //se copian cantTX chars del mensaje
				if(  (uartWasSleeping || ~(p2uart->C2 & UART_C2_TCIE_MASK) ) && (cantTX > 0)) //si la uart estaba durmiendo desde un principio o estaba activaba pero ahora ya termino
				{
					p2uart->C2 |= UART_C2_TE_MASK;
					p2uart->C2 |= UART_C2_TIE_MASK; //habilito la interrupcion para el fin de transmisión

				}
			}
			else
			{
				cantTX = 0;
			}
		}
	}
	return cantTX;
}

uint8_t uartIsTxMsgComplete(uint8_t id)
{
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	uint8_t ret = 0;
	if ((id >= 0) && (id < UART_CANT_IDS)) //solo se hace algo si el id es válido
	{
		p2uart = p2uartsArray[id];
		ret = p2uart->C2 & UART_C2_TCIE_MASK;
	}
	return ret;
}

void resetRXbuffer(uint8_t id)
{
	if(uartMode[id] != BLOCKING)
	{
		markersRXbuffer[id] = 0;
	}
}

uint8_t isRXrejected(uint8_t id)
{
	return countersRXfailed[id];
}

void copyTXmsg(uint8_t id, const char * msg, uint8_t cant)
{
	uint8_t i = 0;
	for(i = 0; i < cant; i++)
	{
		TXbuffers[id][inMarkersTXbuffer[id]] = msg[i];
		if(inMarkersTXbuffer[id] < TX_BUFFER_LEN - 1)
		{
			inMarkersTXbuffer[id]++;  //aumento el in siempre que no llegue al final del arreglo
		}
		else
		{
			inMarkersTXbuffer[id] = 0; //si llego al final del arreglo vuelvo al principio
		}
		(lensTXbuffer[id])++;
	}
}



void UARTX_LON_IRQHandler(uint8_t id)
{

}
void UARTX_RX_TX_IRQHandler(uint8_t id)
{
	//tengo que ver que me llamo a la interrupción
	//para esto, leo el status
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	p2uart = p2uartsArray[id];

	uint8_t i = 0, limitFIFOK64 = 0, limitFIFOsoft = 0;
	if(p2uart->S1 & UART_S1_RDRF_MASK) //recibi data
	{
		if(!flagsReading[id])
		{
			if(p2uart->PFIFO & UART_PFIFO_RXFE_MASK) //si implemento FIFO interna del microcontrolador, defino los limites de cuantos datos voy a leer
			{
				limitFIFOK64 = p2uart->RCFIFO;
				limitFIFOsoft = RX_BUFFER_LEN - 1 - markersRXbuffer[id];
			}
			do
			{
				if(markersRXbuffer[id] < RX_BUFFER_LEN - 1)
				{
					(markersRXbuffer[id])++;   //preparo el marker para escribir. El marker queda apuntando a la posición del último dato ingresado.
					RXbuffers[id][(markersRXbuffer[id])] = p2uart->D;
					countersRXfailed[id] = 0;;
				}
				else
				{
					(countersRXfailed[id])++;
				}
			}while((i < limitFIFOK64) && (i < limitFIFOsoft));   //se hace una sola vez si no esta implementada FIFO del micro. Si no, se repite segun cuanto haya en la FIFO

		}
		else
		{
			//estaban leyendo el buffer que quiero escribir, asi que tendria que avisar que me manden de nuevo lo que me querian mandar.
		}
	}
	else if(p2uart->S1 & UART_S1_TDRE_MASK) //se termino una transmisión
	{
		if(lensTXbuffer[id] > 0) //si queda algo por transmitir
		{
			if(p2uart->PFIFO & UART_PFIFO_TXFE_MASK)
			{
				limitFIFOK64 = TX_FIFO_SIZE_K64 - p2uart->TCFIFO;
				limitFIFOsoft = lensTXbuffer[id];
			}
			do
			{
				p2uart->D = TXbuffers[id][outMarkersTXbuffer[id]]; //transmito otro char del buffer
				if(outMarkersTXbuffer[id] < TX_BUFFER_LEN -1) //actualizo el outMarker
				{
					(outMarkersTXbuffer[id])++;
				}
				else
				{
					outMarkersTXbuffer[id] = 0;
				}
				lensTXbuffer[id]--; //actualizo el len ya que termino un mensaje
				i++;
			} while((i < limitFIFOK64) && (i < limitFIFOsoft));  //se repite el bloque solo si esta implementada la fifo de k64

		}
		else //si no queda nada por transmitir deshabilito transmisión
		{
			p2uart->C2 = (p2uart->C2 & (~UART_C2_TE_MASK));
			p2uart->C2 = p2uart->C2 & (~UART_C2_TIE_MASK);

		}

	}
}


void UARTX_ERR_IRQHandler(uint8_t id)
{

}

void UART0_LON_IRQHandler(void)
{
	UARTX_LON_IRQHandler(U0);
}
void UART0_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U0);
}
void UART0_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U0);
}
void UART1_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U1);
}
void UART1_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U1);
}
void UART2_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U2);
}
void UART2_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U2);
}
void UART3_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U3);
}
void UART3_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U3);
}
void UART4_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U4);
}
void UART4_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U4);
}

void UART_setParity(UART_Type * p2uart, uint8_t parity)
{
	switch(parity)
	{
		case ODD_PARITY_UART:
			p2uart->C1 |= UART_C1_PE_MASK;
			p2uart->C1 |= UART_C1_PT_MASK;
			break;
		case EVEN_PARITY_UART:
			p2uart->C1 |= UART_C1_PE_MASK;
			p2uart->C1 &= (~UART_C1_PT_MASK);
			break;
		case NO_PARITY_UART:
			p2uart->C1 &= (~UART_C1_PE_MASK);
			break;
	}
}


void UART_setBaudRate(UART_Type * p2uart, uint32_t baudRate)
{
	uint8_t brfa;
	uint16_t sbr;
	uint32_t clock;

	clock = ((p2uart == UART0) || (p2uart == UART1))? (__CORE_CLOCK__) : (__CORE_CLOCK__ >> 1); //CORE_CLOCK o BUS_CLOCK

	if((baudRate <= MIN_BAUD_RATE) || (baudRate >= MAX_BAUD_RATE))
	{
		baudRate = BAUD_RATE_DEFAULT;    //si el baudRate ingresado no es valido
	}									//lo cambio por uno valido por default

	sbr = clock / (baudRate << 4); //clock /(baudRate * 16)
	brfa = ((clock << 1) / baudRate) - (sbr << 5); //(2*clock/baudRate) - 32*sbr

	p2uart->BDH = UART_BDH_SBR((sbr >> 8)); //primero se escribe parte alta
	p2uart->BDL = UART_BDL_SBR(sbr);  //luego se escribe parte baja
	p2uart->C4 = (p2uart->C4 & (~UART_C4_BRFA_MASK)) | UART_C4_BRFA(brfa);

}

void UART_setNdataBits(UART_Type * p2uart, uint8_t nBits)
{
	switch(nBits)
	{
		case NBITS_8:
			p2uart->C1 &= (~UART_C1_M_MASK);
			break;
		case NBITS_9:
			p2uart->C1 |= UART_C1_M_MASK;
			break;
		default:
			p2uart->C1 &= (~UART_C1_M_MASK);
			break;
	}
}

void UART_clockGating(uint8_t id)
{
	SIM_Type * sim = SIM;
	switch(id)
	{
		case U0:
			sim->SCGC4 |= SIM_SCGC4_UART0_MASK;
			break;
		case U1:
			sim->SCGC4 |= SIM_SCGC4_UART1_MASK;
			break;
		case U2:
			sim->SCGC4 |= SIM_SCGC4_UART2_MASK;
			break;
		case U3:
			sim->SCGC4 |= SIM_SCGC4_UART3_MASK;
			break;
		case U4:
			sim->SCGC1 |= SIM_SCGC1_UART4_MASK;
			break;
	}
}
