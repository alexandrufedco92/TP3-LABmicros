/*
 * CMP.c
 *
 *  Created on: Oct 21, 2019
 *      Author: Lu
 */

#include "MK64F12.h"
#include "CMP.h"
#include "gpio.h"

#define CMP_ANALOG_IN 1U // PTC7, CMP0_IN1				//PTC3, CMP1_IN1
#define CMP_ANALOG_REF 7U	//elijo dac
#define HYST_MASK 3U
#define DAC_VALUE 31U
#define VREF_SOURCE_VINX 0U

							//CMP0_IN1			//CMP1_IN1
uint8_t CMP_in_pins[] = {PORTNUM2PIN(PC, 7), PORTNUM2PIN(PC, 3)};

							//CMP0_OUT			//CMP1_OUT
uint8_t CMP_out_pins[] = {PORTNUM2PIN(PC, 5), PORTNUM2PIN(PC, 4)};


CMP_Type * arrayP2CMP[] = CMP_BASE_PTRS;
IRQn_Type arrayCMPirqs[] = CMP_IRQS;
CMP_Type* base;

static void clockGating();
static void enableInterrupts();
static void configurePins(uint8_t id);
void initCMP(cmps_ids id);
static void configureInputs(uint8_t positiveInput, uint8_t negativeInput);
static void configureDAC();




void initCMP(cmps_ids id){

	base = arrayP2CMP[id];
	clockGating();
	configurePins(id);		//solo CMP0

	//para que la salida salga por el pin
	base->CR1 |= CMP_CR1_OPE_MASK;

	//histeresis+
	base->CR0 |= CMP_CR0_HYSTCTR(HYST_MASK);

	configureDAC();
	configureInputs(CMP_ANALOG_IN, CMP_ANALOG_REF);

	//setEnableDMA()
	base->CR1 |= CMP_CR1_EN_MASK;		//enable module
	//enableInterrupts();
	//NVIC_EnableIRQ(arrayCMPirqs[id]);
}


void clockGating(){
	SIM->SCGC4 |= SIM_SCGC4_CMP_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

}

void configurePins(uint8_t id){
	uint8_t cmp_in_mux = 0;
	uint8_t cmp_out_mux = 6;
	switch(id)
	{
	case CMP_0:
		cmp_in_mux = 0;
		cmp_out_mux = 6;
		break;
	case CMP_1:
		cmp_in_mux = 0;
		cmp_out_mux = 6;
		break;
	}
	PORT_Type * portPointers[] = PORT_BASE_PTRS;

	uint8_t port_in = PIN2PORT(CMP_in_pins[id]);
	uint8_t num_pin_in= PIN2NUM(CMP_in_pins[id]);

	uint8_t port_out = PIN2PORT(CMP_out_pins[id]);
	uint8_t num_pin_out = PIN2NUM(CMP_out_pins[id]);

	setPCRmux(portPointers[port_in], num_pin_in, cmp_in_mux);
	setPCRmux(portPointers[port_out], num_pin_out, cmp_out_mux);
	setPCRirqc(portPointers[port_in], num_pin_in, GPIO_IRQ_MODE_DISABLE); //deshabilito interrupciones de puerto
	setPCRirqc(portPointers[port_out], num_pin_out, GPIO_IRQ_MODE_DISABLE);
}


void enableInterrupts(){
	base->SCR |= (CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK);
}

void configureInputs(uint8_t positiveInput, uint8_t negativeInput){

	uint8_t mask = base->MUXCR;

	//borro los selectores que tiene
	mask &= ~(uint8_t)(CMP_MUXCR_PSEL_MASK | CMP_MUXCR_MSEL_MASK);
	mask |= (CMP_MUXCR_MSEL(negativeInput) | CMP_MUXCR_PSEL(positiveInput));
	base->MUXCR = mask;
}


void configureDAC(){

	base->DACCR |= (CMP_DACCR_VOSEL(DAC_VALUE) | CMP_DACCR_DACEN_MASK | CMP_DACCR_VRSEL_MASK);
}



void CMP0_IRQHandler(void){
	CMP_Type* base = arrayP2CMP[CMP_0];
	base->SCR |= (CMP_SCR_CFF_MASK | CMP_SCR_CFR_MASK);

}
