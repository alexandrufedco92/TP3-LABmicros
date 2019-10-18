/*
 * timer.h
 *
 *  Created on: Aug 31, 2019
 *      Author: G5
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdbool.h>
#include <stdint.h>


/******************************************************************************************
 *  				TIPOS DE DATOS, CONSTANTES Y VARIABLES GLOBALES
 ******************************************************************************************/
typedef enum {INACTIVITY, MAX_FPS_CLEAR, TIME_OUT_PC_ANSWER, REFRESH_PC, REFRESH_COORD, NUM_TIMERS}timerUd_t; //Timers utilizados
typedef void (*callback_ptr)(void);


/********************************************************************************************
 * 										FUNCIONES
 ********************************************************************************************/
void InitializeTimers(void);
//Recibe el timeout en milisegundos
void SetTimer(unsigned char index,unsigned long int timeout, callback_ptr func);
void DisableTimer(unsigned char index);
void EnableTimer(unsigned char index);
void RestartTimer(unsigned char index);
unsigned int GetTimerCount(unsigned char index); //Devuelve cuanto tiempo, en ms, paso desde que se
												//setteo el timer recibido

#endif /* TIMER_H_ */
