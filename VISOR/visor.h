#ifndef VISOR_H_
#define VISOR_H_

#include <stdio.h>

typedef struct visor Visor;

struct visor {
	//PROPIEDADES
	FILE* FILE;
	char* PATH;
};

//CONSTRUCTOR
Visor* new_visor(char* PATH);

#endif /* VISOR_H_ */
