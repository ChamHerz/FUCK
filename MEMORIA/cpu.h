#ifndef CPU_H_
#define CPU_H_

#include <pthread.h>

typedef struct cpu Cpu;

struct cpu {
	//PROPIEDADES
	int descriptor;
	int accion;
	int ocupado;
	pthread_t hilo_para_cpu;
};

//Constructor
Cpu* new_cpu(int descriptor);

//Destructor
void destroy_cpu(Cpu* cpu);

#endif /* CPU_H_ */
