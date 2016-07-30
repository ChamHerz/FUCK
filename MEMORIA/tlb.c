#include "tlb.h"
#include <stdlib.h>

//constructor
Tlb* new_tlb(int PID,int NRO_PAGINA,int MARCO_ID){
	Tlb* t;
	t = malloc(sizeof(struct tlb));
	t->PID = PID;
	t->NRO_PAGINA = NRO_PAGINA;
	t->NRO_MARCO = MARCO_ID;
	t->PRESENTE = 1;
	t->REFERENCIADA = 1;
	return t;
}

//Destructor
void destroy_tlb(Tlb* tlb){
	free(tlb);
}
