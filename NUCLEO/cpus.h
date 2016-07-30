#ifndef CPUS_H_
#define CPUS_H_

#include "cpu.h"
#include <commons/collections/list.h>

typedef struct cpus Cpus;

struct cpus {
	//PROPIEDADES
	t_list* lista;

	//Semaforos
	pthread_mutex_t sem_cpus_estados;
};

Cpus* new_cpus();

//Metodos
void destroy_cpus(Cpus* cpus);

void agregar_cpu(Cpus* cpus, Cpu* cpu);

Cpu* get_cpu(Cpus* cpus,int index);

Cpu* buscar_cpu(Cpus* cpus, int descriptor);

void set_accion_cpu(Cpus* cpus,int descriptor, int accion);

int cantidad_cpus(Cpus* cpus);

Cpu* buscar_cpu_osiosa(Cpus* cpus);

void set_id_process(Cpus* cpus,int descriptor,int ID_PROCESS);

bool algun_cpu_osioso(Cpus* cpus);

#endif /* CPUS_H_ */
