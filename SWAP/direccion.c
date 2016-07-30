#include "direccion.h"
#include <stdlib.h>

Direccion* new_direccion(int byte_start, int byte_cant, int pag_size){
	Direccion* d;
	d = malloc(sizeof(struct direccion));
	//Propiedades
	d->PAG = byte_start / pag_size;
	d->OFFSET = byte_start % pag_size;
	d->BUFFER = byte_cant;
	return d;
}

Direccion* new_direccion_vacia(){
	Direccion* d;
	d = malloc(sizeof(struct direccion));
	return d;
}
