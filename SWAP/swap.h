#ifndef SWAP_H_
#define SWAP_H_

#include <pthread.h>
#include "bitmap.h"
#include "programas.h"
#include "ansisop.h"
#include "log.h"

typedef struct swap Swap;

struct swap {
	//PROPIEDADES
	char* PUERTO;
	int DESCRIPTOR;
	int DESCRIPTOR_MEMORIA;
	char* NOMBRE_SWAP;
	int CANT_PAGINAS;
	int SIZE_PAGINA;
	int RETARDO_COMPACTACION;
	int RETARDO_ACCESSO;

	//MIS PROPIEDADES
	char* path_conf_swap;
	Bitmap* BITMAP;
	Programas* programas;
	int MONITOR_ACTIVADO;
	Log* FILE_MONITOR;

	//HILOS
	pthread_t hilo_para_memoria;

	//SEMAFOROS
	pthread_mutex_t sem_memoria;
};

Swap* new_swap();

void destroy_swap(Swap* swap);

void set_path_archivo_configuracion(Swap* swap,char* PATH_SWAP_CONF);

void cargar_archivo_configuracion(Swap* swap);

void swap_iniciar(Swap* swap);

void hilo_monitor(Swap* swap);

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Swap*),Swap* swap);

void crear_hilo_un_para_un_monitor(void(*funcion_externa)(Swap*),Swap* swap);

void hilo_para_memoria(Swap* swap);

void swap_crear_bitmap(Swap* swap);

void swap_crear_archivo(Swap* swap);

void recibir_size_ansisop(Swap* swap);

void recibir_direccion_memoria(Swap* swap);

int realizar_accion_con_memoria(Swap* swap,int dato_leido);

int calcular_cant_pag_necesarias(Swap* swap,Ansisop* ansisop);

void escribir_swap(char* path,Ansisop* ansisop,Programa* programa);

void compactar_swap(Swap* swap);

#endif /* SWAP_H_ */
