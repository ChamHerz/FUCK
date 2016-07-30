#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <pthread.h>
#include "interprete.h"

typedef struct consola Consola;

struct consola {
	//PROPIEDADES
	char* IP_NUCLEO;
	char* PUERTO_NUCLEO;
	char* PATH;
	int DESCRIPTOR_SERVIDOR;
	char* PATH_ANSISOP;

	//HILOS
	//pthread_t hilo_para_nucleo;
	//pthread_t hilo_para_interprete;
};

Consola* new_consola();

void destroy_consola(Consola* consola);

void consola_inciar(Consola* consola);

void set_path_archivo_configuracion(Consola* consola,char* path);

void cargar_archivo_configuracion(Consola* consola);

int ejecutar_comando(Consola* consola);

int comando_fin(Consola* consola);

int comando_ansisop(Consola* consola);

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Consola*),Consola* consola);

#endif /* CONSOLA_H_ */
