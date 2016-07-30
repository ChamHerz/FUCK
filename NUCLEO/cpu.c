#include "cpu.h"
#include <stdlib.h>

//constructor
Cpu* new_cpu(int descriptor){
	Cpu* c;
	c = malloc(sizeof(struct cpu));
	c->descriptor = descriptor;
	//c->ocupado = 0;  //Si esta operando con el Nucleo o Memoria
	c->osiosa = 1; //Si no tiene ningun progrma, arranca osiosa porque se pasa el PagSize y quantum sleep
	//c->evento_programa_bloqueado = 0;
	c->PID = 0; //Arranca sin PID
	//c->evitar_kill_pid = 0;
	return c;
}

//Destructor
void destroy_cpu(Cpu * cpu){
	free(cpu);
}
