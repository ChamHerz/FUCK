#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <pthread.h>

typedef struct semaforo Semaforo;

struct semaforo {
	//PROPIEDADES
	char id_semaforo;
	int valor_semaforo;

	//MUTEX
	pthread_mutex_t MUTEX;
};
//Constructor
Semaforo* new_semaforo(char id_semaforo,int valor_semaforo);

//Destructor
void destroy_semaforo(Semaforo* semaforo);

#endif /* SEMAFORO_H_ */
