#include "programa.h"
#include <stdlib.h>
//Metodos
void calcular_cant_pag(Programa* programa,int size_pag){
	programa->CANT_PAG = programa->SIZE / size_pag;
	if (programa->SIZE%size_pag != 0){
		programa->CANT_PAG++;
	}
	programa->CANT_PAG = programa->CANT_PAG + programa->PAG_STACK;
}

//constructor
Programa* new_programa(){
	Programa* p;
	p = malloc(sizeof(struct programa));
	return p;
}

//Destructor
void destroy_programa(Programa* programa){
	free(programa);
}
