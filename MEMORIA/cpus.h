#ifndef CPUS_H_
#define CPUS_H_

#include "cpu.h"
#include <commons/collections/list.h>

typedef struct cpus Cpus;

struct cpus {
	//PROPIEDADES
	t_list* lista;
};

Cpus* new_cpus();

void destroy_cpus(Cpus* cpus);

void agregar_cpu(Cpus* cpus, Cpu* cpu);

Cpu* get_cpu(Cpus* cpus,int index);

Cpu* buscar_cpu(Cpus* cpus, int descriptor);

void set_accion_cpu(Cpus* cpus,int descriptor, int accion);

int cantidad_cpus(Cpus* cpus);

#endif /* CPUS_H_ */
