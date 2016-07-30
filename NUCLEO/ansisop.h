#ifndef ANSISOP_H_
#define ANSISOP_H_

#include "consola.h"

typedef struct ansisop Ansisop;

struct ansisop {
	//PROPIEDADES
	int PAG_STACK;
	int PID;
	int SIZE;
	char* CADENA;
};

#endif /* ANSISOP_H_ */
