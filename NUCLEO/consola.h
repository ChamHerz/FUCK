#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <pthread.h>

typedef struct consola Consola;

struct consola {
	//PROPIEDADES
	pthread_t hilo_consola;
	int id_consola;
	int descriptor;
	int accion;
	int ocupada;
	int PID;
};

//Constructor
Consola * new_consola(int descriptor);

//Destructor
void destroy_consola(Consola * this);

#endif /* CONSOLA_H_ */
