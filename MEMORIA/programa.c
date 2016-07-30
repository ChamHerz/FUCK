#include "programa.h"
#include <stdlib.h>

//constructor
Programa* new_programa(int PID){
	Programa* p;
	p = malloc(sizeof(struct programa));
	p->PID = PID;
	p->CANT_MARCOS = 0;
	return p;
}

//Destructor
void destroy_programa(Programa* programa){
	free(programa);
}
