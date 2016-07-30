#include "semaforos.h"
#include <stdlib.h>
//constructor
Semaforos* new_semaforos(){
	Semaforos* s;
	s = malloc(sizeof(struct semaforos));
	s->lista = list_create();
	return s;
}

//Destructor
void destroy_semaforos(Semaforos* semaforos){
	free(semaforos);
}

void agregar_semaforo(Semaforos* semaforos, Semaforo* semaforo){
	list_add(semaforos->lista,semaforo);
}

Semaforo* buscar_semaforo(Semaforos* semaforos, char id_semaforo){
	bool condicion_busqueda(Semaforo* semaforo){
		return semaforo->id_semaforo == id_semaforo;
	}

	return list_find(semaforos->lista,(void*)condicion_busqueda);
}
