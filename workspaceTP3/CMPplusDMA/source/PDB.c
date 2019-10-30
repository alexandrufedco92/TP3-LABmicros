/*
 * PDB.c
 *
 *  Created on: Oct 30, 2019
 *      Author: Lu
 */
#include "MK64F12.h"
#include "PDB.h"

static void clockGating(void);



void initPDB(void){
	clockGating();

}


void clockGating(void){
	SIM->SCGC6 |= SIM_SCGC6_PDB_MASK;
}
