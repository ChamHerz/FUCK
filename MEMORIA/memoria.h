#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <pthread.h>
#include "cpus.h"
#include "marcos.h"
#include "programas.h"
#include "direccion.h"
#include "tlbs.h"
#include "log.h"
#include "interprete.h"

typedef struct memoria Memoria;

struct memoria {
	//PROPIEDADES
	char* PUERTO;
	char* PUERTO_NUCLEO;
	char* IP_SWAP;
	char* PUERTO_SWAP;
	int MARCOS;
	int MARCO_SIZE;
	int MARCO_X_PROC;
	int ALGORITMO;
	int RETARDO;
	int ENTRADAS_TBL;
	int DESCRIPTOR_SWAP;
	int DESCRIPTOR_NUCLEO;

	char* path_conf_memoria;

	//MODULOS
	char** VECTOR_MEMORIA;
	Cpus* cpus;
	Marcos* marcos;
	Programas* programas;
	Tlbs* tlbs;
	int MONITOR_ACTIVADO;
	Log* FILE_MONITOR;
	Log* FILE_DUMP_ALL;
	Log* FILE_DUMP_ONE;
	Log* FILE_SHOW_ALL;
	Interprete* interprete;

	//HILOS
	pthread_t hilo_para_nucleo;
	pthread_t hilo_para_swap;
	pthread_t hilo_para_cpus;
	pthread_t hilo_para_interprete;

	//SEMAFOROS
	pthread_mutex_t sem_swap;
	pthread_mutex_t sem_listas;
};

typedef struct memoriacpu MemoriaCpu;

struct memoriacpu {
	Memoria* memoria;
	Cpu* cpu;
};

Memoria* new_memoria();

void destroy_memoria(Memoria* memoria);

void memoria_inciar(Memoria* memoria);

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Memoria*),Memoria* memoria);

pthread_t crear_hilo_para_un_cpu(void(*funcion_externa)(MemoriaCpu*),MemoriaCpu* memoriacpu);

void hilo_monitor(Memoria* memoria);

void hilo_para_interprete(Memoria* memoria);

void hilo_para_nucleo(Memoria* memoria);

void hilo_para_swap(Memoria* memoria);

void hilo_para_cpus(Memoria* memoria);

void hilo_para_un_cpu(MemoriaCpu* memoriacpu);

void recibir_size_ansisop(Memoria* memoria);

void recibir_direccion_del_cpu(Memoria* memoria, Cpu* cpu);

void recibir_direccion_para_almacenar_del_cpu(Memoria* memoria,Cpu* cpu);

char* buscar_direccion_en_memoria(Memoria* memoria,Direccion* direccion,int escribir);

char* buscar_direccion_en_tlb(Memoria* memoria, Direccion* direccion, int escribir,int* pro_retardo);

char* leer_marco(Memoria* memoria,Direccion* direccion,Marco* marco);

char* leer_pagina_swap(Memoria* memoria,Direccion* direccion);

#endif /* MEMORIA_H_ */
