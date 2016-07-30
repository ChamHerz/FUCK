#include "semaforo.h"
#include <stdlib.h>

//constructor
Semaforo* new_semaforo(char id_semaforo,int valor_semaforo){
	Semaforo* s;
	s = malloc(sizeof(struct semaforo));
	s->id_semaforo = id_semaforo;
	s->valor_semaforo = valor_semaforo;

	//MUTEX
	pthread_mutex_init(&s->MUTEX, NULL);
	return s;
}

//Destructor
void destroy_semaforo(Semaforo* semaforo){
	free(semaforo);
}
