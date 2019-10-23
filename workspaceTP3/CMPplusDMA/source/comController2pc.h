/*
 * comController2pc.h
 *
 *  Created on: 4 oct. 2019
 *      Author: G5
 */

#ifndef COMCONTROLLER2PC_H_
#define COMCONTROLLER2PC_H_

//specification of the protocol that will be implemented using this file
#define MAX_TIME_REACTION 100//miliseconds
#define MAX_ERRORS_ALLOWED 1 //max number of tries to communicate

#define MAX_LEN_COORD 4

//initResourcesController2node:
//args: implementationOption: it has to be one of the implementation options defined in this
//							file, and it must be equal to the implementation on the other side of communication bus.
//		dataMaxLen: maximum size of the data sent in a frame
//		p2config: p2config.
//brief: initialization of necessary resources for communication (initialize the implementation option
//		and establish a limit for dataLen).
void initResourcesController2pc(void);

//sendMessage2node:
//args: id:
//		p2coord:
//		coordName:
//		cant:
//return: void
//brief: This function create a package a establishes a  new communication as master.
//		If the slave doesn't react with an "acknowledge" after a period of time (MAX_TIME_REACTION)
//		or it reacts by reporting an error, the package will be sent again
//		and the process can be repeated a limited number of times (MAX_ERRORS_ALLOWED)
void sendMessage2pc(char idBoard, char * p2coord, char coordName, int cant);



#endif /* COMCONTROLLER2PC_H_ */
