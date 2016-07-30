#include "cpus.h"
#include <stdlib.h>
//constructor
Cpus* new_cpus(){
	Cpus* c;
	c = malloc(sizeof(struct cpus));
	c->lista = list_create();

	//semaforos
	pthread_mutex_init(&c->sem_cpus_estados, NULL);

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
	if (cpu->ocupado == 0){
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

void set_id_process(Cpus* cpus,int descriptor,int ID_PROCESS){
	Cpu* cpu = buscar_cpu(cpus,descriptor);
	cpu->ID_PROCESS = ID_PROCESS;
}

void set_ocupado_cpu(Cpus* cpus,int descriptor){
	Cpu* cpu = buscar_cpu(cpus,descriptor);
	cpu->ocupado = 1;
}

void borrar_cpu(Cpus* cpus, int descriptor){
	bool condicion_borrado(Cpu* cpu){
		return cpu->descriptor == descriptor;
	}

	Cpu* cpu;
	list_remove_and_destroy_by_condition(cpus->lista,(void*)condicion_borrado,destroy_cpu);
}

int cantidad_cpus(Cpus* cpus){
	return list_size(cpus->lista);
}

bool algun_cpu_osioso(Cpus* cpus){
	bool condicion_busqueda(Cpu* cpu){
		return cpu->osiosa == 1;
	}
	return list_any_satisfy(cpus->lista,(void*)condicion_busqueda);
}

Cpu* buscar_cpu_osiosa(Cpus* cpus){
	bool condicion_busqueda(Cpu* cpu){
		return cpu->osiosa == 1;
	}

	return list_find(cpus->lista,(void*)condicion_busqueda);
}

//Funcion utilizada para cadena "PID1, PID2, etc"
char* recorrer_cpus(Cpus* cpus){
	pthread_mutex_lock(&cpus->sem_cpus_estados); //varios hilos muestran el visor
	char* cadena_PID = string_new();

	void funcion_closure(Cpu* cpu){
		char* DESCRIPTOR = string_itoa(cpu->descriptor);
		char* OCUPADO = string_itoa(cpu->ocupado);
		char* OSIOSA = string_itoa(cpu->osiosa);
		char* PID = string_itoa(cpu->PID);
		char* ID_PROCESS = string_itoa(cpu->ID_PROCESS);
		string_append(&cadena_PID,DESCRIPTOR);
		string_append(&cadena_PID,"      ");
		string_append(&cadena_PID,OCUPADO);
		string_append(&cadena_PID,"        ");
		string_append(&cadena_PID,OSIOSA);
		string_append(&cadena_PID,"         ");
		string_append(&cadena_PID,"PID");
		string_append(&cadena_PID,PID);
		string_append(&cadena_PID,"   ");
		string_append(&cadena_PID,ID_PROCESS);
		string_append(&cadena_PID,"\n");
	}

	list_iterate(cpus->lista,(void*)funcion_closure);

	pthread_mutex_unlock(&cpus->sem_cpus_estados);
	return cadena_PID;
}
