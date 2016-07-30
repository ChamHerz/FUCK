#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

#include "semaforo.h"
#include <commons/collections/list.h>

typedef struct semaforos Semaforos;

struct semaforos {
	//PROPIEDADES
	t_list* lista;
};

Semaforos* new_semaforos();

void destroy_semaforos(Semaforos* semaforos);

void agregar_semaforo(Semaforos* semaforos, Semaforo* semaforo);

Semaforo* buscar_semaforo(Semaforos* semaforos, char id_semaforo);

#endif /* SEMAFOROS_H_ */
