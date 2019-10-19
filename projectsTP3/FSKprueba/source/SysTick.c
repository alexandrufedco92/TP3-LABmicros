
#include "SysTick.h"
#include "ADC.h"
#include <stdio.h>
#include "UART.h"

void ADC_PISR (void);
__ISR__ SysTick_Handler (void)
{
	ADC_PISR();
}

void ADC_PISR (void)
{

#define ADC_0_DP0_CHANNEL 0x00

char mess[30];
char *pmess=mess;
ADCData_t data;


	ADC_Start(ADC0, ADC_0_DP0_CHANNEL, ADC_mA);

	while(!ADC_IsReady(ADC0));
	data = ADC_getData(ADC0);

	sprintf(mess,"Mes:%d\n",data);
	pmess=mess;
	while(*pmess)
	UART_Send_Data(*pmess++);


}


void SysTick_Init (void)
{
	SysTick->CTRL = 0x00;
	SysTick->LOAD = 12500000L-1; // =125ms
	SysTick->VAL  = 0x00;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}


