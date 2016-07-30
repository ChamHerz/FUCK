#ifndef TLBS_H_
#define TLBS_H_

#include "tlb.h"
#include <commons/collections/list.h>
#include <pthread.h>

typedef struct tlbs Tlbs;

struct tlbs {
	//PROPIEDADES
	int TLBID;
	int CANT_ENTRADAS;
	t_list* lista;

	pthread_mutex_t sem_tlb_visor;
};

Tlbs* new_tlbs();

void destroy_tlbs(Tlbs* tlbs);

void agregar_tlb(Tlbs* tlbs, Tlb* tlb);

Tlb* buscar_tlb(Tlbs* tlbs, int NRO_PAG, int PID);

char* recorrer_tabla_tlbs(Tlbs* tlbs);

void borrar_tlb_completa(Tlbs* tlbs);

#endif /* TLBS_H_ */
