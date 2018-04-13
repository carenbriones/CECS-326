/*
 * registration.h
 *
 *  Created on: Apr 6, 2017
 *      Author: carenbriones
 */

#define REGISTRATION_H_

/*	Header file to be used with

 *	shmp1.c and shmc1.c

 */

struct CLASS {
	// Class number data member
	char class_number[6];
	// Date data member
	char date[7];
	// Title of the class
	char title[50];
	// Number of seats left
	int seats_left;

};
