#include "consola.h"

//constructor
Consola * new_consola(int descriptor){
	Consola * c;
	c = malloc(sizeof(struct consola));
	c->descriptor = descriptor;
	c->ocupada = 0;
	c->PID = 0;
	return c;
}

//Destructor
void destroy_consola(Consola * this){
	free(this);
}
