#include "pcb.h"

//constructor
Pcb* new_pcb(){
	Pcb* p;
	p = malloc(sizeof(struct pcb));
	p->PC = -1;
	p->FLAG_SISGUR1 = 0;
	return p;
}

void destroy_pcb(Pcb* pcb){
	free(pcb);
}
