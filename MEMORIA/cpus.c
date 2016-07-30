#include "cpus.h"
#include <stdlib.h>
//constructor
Cpus* new_cpus(){
	Cpus* c;
	c = malloc(sizeof(struct cpus));
	c->lista = list_create();
	return c;
}

//Destructor
void destroy_cpus(Cpus* cpus){
	free(cpus);
}

void agregar_cpu(Cpus* cpus, Cpu* cpu){
	list_add(cpus->lista,cpu);
}

int esta_ocupado(Cpus* cpus,int descriptor){
	Cpu* cpu = buscar_cpu(cpus,descriptor);
	int estado = cpu->ocupado;
	if(cpu->ocupado==0){
		cpu->ocupado = 1;
	}
	return estado;
}

Cpu* get_cpu(Cpus* cpus,int index){
	Cpu* cpu;
	return list_get(cpus->lista,index);
}

Cpu* buscar_cpu(Cpus* cpus, int descriptor){
	bool condicion_busqueda(Cpu* cpu){
		return cpu->descriptor == descriptor;
	}

	return list_find(cpus->lista,(void*)condicion_busqueda);
}

void set_accion_cpu(Cpus* cpus,int descriptor, int accion){
	Cpu* cpu = buscar_cpu(cpus,descriptor);
	cpu->accion = accion;
}

void borrar_cpu(Cpus* cpus, int descriptor){
	bool condicion_borrado(Cpu* cpu){
		return cpu->descriptor == descriptor;
	}

	Cpu* cpu;
	cpu = list_remove_by_condition(cpus->lista,(void*)condicion_borrado);
}

int cantidad_cpus(Cpus* cpus){
	return list_size(cpus->lista);
}
