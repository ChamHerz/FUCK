#ifndef NUCLEO_H_
#define NUCLEO_H_

#include <pthread.h>
#include "consolas.h"
#include "pcbs.h"
#include "ansisop.h"
#include "cpus.h"
#include "variables.h"
#include "semaforos.h"
#include "ios.h"
#include "log.h"
#include <commons/config.h>

#define BLOQUEO_WAIT 1
#define FINAL_PROGRAMA 2
#define QUANTUM_COMPLETO 3
#define BLOQUEO_IO 4
#define SEGMENTO_FAULT 5

typedef struct nucleo Nucleo;

struct nucleo {
	//PROPIEDADES
	int PUERTO_PROG;
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	int PUERTO_CPU;
	int SIZE_PAGINA;
	int PAG_STACK;
	int QUANTUM;
	int QUANTUM_SLEEP;
	char** SEM_ID;
	char** SEM_INIT;
	char** IO_ID;
	char** IO_SLEEP;
	char** SHARED_VARS;

	//MIS PROPIEDADES
	char* path_conf_nucleo;
	int DESCRIPTOR_MEMORIA;
	int PID_ACTUAL;
	t_config* FILE_CONFIGURACION;
	int MONITOR_ACTIVADO;
	Log* FILE_MONITOR;

	//COLAS
	Pcbs* cola_nuevos;
	Pcbs* cola_listos;
	Pcbs* cola_ejecutando;
	Pcbs* cola_bloqueados;
	Pcbs* cola_finalizados;
	Pidblocks* lista_bloqueados;

	//MODULOS
	Consolas* consolas;
	Cpus* cpus;
	Variables* variables;
	Semaforos* semaforos;
	Ios* ios;

	//HILOS
	pthread_t hilo_para_consolas;
	pthread_t hilo_para_memoria;
	pthread_t hilo_para_cpus;
	pthread_t hilo_para_round_robin;
	pthread_t hilo_para_cambio_configuracion;

	//SEMAFOROS
	pthread_mutex_t sem_memoria;
	pthread_mutex_t sem_pcb;
	pthread_mutex_t sem_cpu;
	pthread_mutex_t sem_semaforos;
	pthread_mutex_t sem_variables_glo;
};

typedef struct nucleoconsola NucleoConsola;

struct nucleoconsola {
	Nucleo* nucleo;
	Consola* consola;
};

typedef struct nucleocpu NucleoCpu;

struct nucleocpu {
	Nucleo* nucleo;
	Cpu* cpu;
};

typedef struct nucleosemaforo NucleoSemaforo;

struct nucleosemaforo {
	Nucleo* nucleo;
	Semaforo* semaforo;
};

typedef struct nucleoio NucleoIo;

struct nucleoio {
	Nucleo* nucleo;
	Io* io;
	Pidblock* pidblock;
};

//Constructor
Nucleo* new_nucleo();

NucleoConsola* new_nucleoconsola();

NucleoCpu* new_nucleocpu();

NucleoSemaforo* new_nucleosemaforo(Nucleo* nucleo, Semaforo* semaforo);

NucleoIo* new_nucleoio(Nucleo* nucleo, Io* io, Pidblock* pidblock);

void set_path_archivo_configuracion(Nucleo* nucleo,char* path);

void cargar_archivo_configuracion(Nucleo* nucleo);

void crear_estructuras_administrativas(Nucleo* nucleo);

void nucleo_iniciar(Nucleo* nucleo);

void crear_pcb(Nucleo* nucleo,Ansisop* ansisop,Pcb* pcb);

void enviar_pcb(Nucleo* nucleo,Cpu* cpu,Pcb* pcb);

Pcb* recibir_pcb(Nucleo* nucleo,Cpu* cpu);

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Nucleo*),Nucleo* nucleo);

void crear_hilo_un_para_un_monitor(void(*funcion_externa)(Nucleo*),Nucleo* nucleo);

void hilo_monitor(Nucleo* nucleo);

void hilo_para_cambio_conf(Nucleo* nucleo);

void hilo_para_consolas(Nucleo* nucleo);

void hilo_para_una_consola(NucleoConsola* nucleoconsola);

void hilo_para_cpus(Nucleo* nucleo);

void hilo_para_un_cpu(NucleoCpu* nucleocpu);

void hilo_para_memoria(Nucleo* nucleo);

void hilo_para_round_robin(Nucleo* nucleo);

int realizar_accion_consola(Nucleo* nucleo, Consola* consola, int accion);

int verificar_swap_espacio(Nucleo* nucleo,Ansisop* ansisop);

int calcular_cant_pag_necesarias(Nucleo* nucleo, int ansisop_size);

int espera_ordenes_cpu(Nucleo* nucleo, Cpu* cpu);

void verificar_motivo_desalojo(Nucleo* nucleo,Cpu* cpu,Pcb* pcb);

int ejecutar_wait(Nucleo* nucleo,Cpu* cpu, char id_semaforo,int pid);

int ejecutar_signal(Nucleo* nucleo,char id_semaforo);

#endif /* NUCLEO_H_ */
