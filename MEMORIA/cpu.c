#include "cpu.h"
#include <stdlib.h>

//constructor
Cpu* new_cpu(int descriptor){
	Cpu* c;
	c = malloc(sizeof(struct cpu));
	c->descriptor = descriptor;
	c->ocupado = 0;
	return c;
}

//Destructor
void destroy_cpu(Cpu * cpu){
	free(cpu);
}
