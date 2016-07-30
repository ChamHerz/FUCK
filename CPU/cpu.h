#ifndef CPU_H_
#define CPU_H_

#include "pcb.h"
#include <pthread.h>
#include "direccion.h"

#define BLOQUEO_WAIT 1
#define FINAL_PROGRAMA 2
#define QUANTUM_COMPLETO 3
#define BLOQUEO_IO 4
#define SEGMENTO_FAULT 5

typedef struct cpu Cpu;

struct cpu {
	//PROPIEDADES
	char* IP_NUCLEO;
	char* PUERTO_NUCLEO;
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* PATH;
	//int QUANTUM_SLEEP;
	int DESCRIPTOR_NUCLEO;
	int DESCRIPTOR_MEMORIA;
	int RESPUESTA_SERVIDOR;
	int OCUPADO;
	char* DATO_ENVIAR;
	int PAG_SIZE;
	int IP_PROCESS;
	Pcb* pcb;

	//MIS PROPIEDADES
	int SEMAFORO_BLOQUEO;
	//int QUANTUM_ACTUAL;

	//EVENTO
	int evento_cierre_por_SISGUR1;
	int evento_finalizar_CPU;

	//HILOS
	pthread_t hilo_para_nucleo;
	pthread_t hilo_para_sigusr1;
};

Cpu* new_cpu();

void destroy_cpu(Cpu* cpu);

void cpu_inciar(Cpu* cpu);

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Cpu*),Cpu* cpu);

void hilo_para_nucleo(Cpu* cpu);

void enviar_pcb_al_nucleo(Cpu* cpu);

Pcb* recepcion_pcb(Cpu* cpu);

void ejecutar_quantum(Cpu* cpu);

int esperar_orden_nucleo(Cpu* cpu);

char* solicitar_a_memoria_direccion(Cpu* cpu,Direccion* direccion);

void almacenar_bytes_en_pag(Cpu* cpu,Direccion* direccion);

void avisar_nucleo_final_programa(Cpu* cpu,Pcb* pcb);

void avisar_nucleo_ejecucion_wait(Cpu* cpu,char semaforo);

void avisar_nucleo_ejecucion_signal(Cpu* cpu,char semaforo);

int avisar_nucleo_variable_global(Cpu* cpu,char* variable_global);

void avisar_nucleo_variable_global_modificada(Cpu* cpu,char* variable_global,int valor);

void avisar_nucleo_imprimir_valor(Cpu* cpu,int valor_variable);

void avisar_nucleo_imprimir_texto(Cpu* cpu,char* cadena);

void avisar_nucleo_ejecucion_io(Cpu* cpu,char* identificador,int tiempo);

#endif /* CPU_H_ */
