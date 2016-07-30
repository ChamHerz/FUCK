#ifndef CPU_H_
#define CPU_H_

#include <pthread.h>

typedef struct cpu Cpu;

struct cpu {
	//PROPIEDADES
	int descriptor;
	int accion;
	int ocupado;
	int osiosa;
	int PAG_SIZE;
	int PID;
	int ID_PROCESS;

	//evento
	int evento_programa_bloqueado;
	int evitar_kill_pid;

	//HILO
	pthread_t hilo_cpu;
};
//Constructor
Cpu* new_cpu(int descriptor);

//Destructor
void destroy_cpu(Cpu* cpu);

#endif /* CPU_H_ */
