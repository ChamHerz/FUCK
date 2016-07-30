#include "pcb.h"

//constructor
Pcb* new_pcb(){
	Pcb* p;
	p = malloc(sizeof(struct pcb));
	p->FLAG_SISGUR1 = 0;
	return p;
}

void destroy_pcb(Pcb* pcb){
	free(pcb);
}

Address* new_address(char id,int pag,int off,int size){
	Address* a;
	a = malloc(sizeof(struct address));
	a->ID = id;
	a->PAG = pag;
	a->OFFSET = off;
	a->SIZE = size;
	return a;
}
