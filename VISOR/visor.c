#include "visor.h"
#include <stdlib.h>

//constructor
Visor* new_visor(char* PATH){
	Visor* v;
	v = malloc(sizeof(struct visor));
	//Propiedades
	v->PATH = PATH;
	return v;
}
