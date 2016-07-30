#include "tlbs.h"
#include <stdlib.h>
#include <pthread.h>

//constructor
Tlbs* new_tlbs(){
	Tlbs* t;
	t = malloc(sizeof(struct tlbs));
	t->lista = list_create();
	t->TLBID = -1;

	pthread_mutex_init(&t->sem_tlb_visor,NULL);

	return t;
}

//Destructor
void destroy_tlbs(Tlbs* tlbs){
	free(tlbs);
}

int conseguir_id_tlb_libre(Tlbs* tlbs){
	int TlbID = 0;

	bool condicion_sort(Tlb* tlb1,Tlb* tlb2){
		return tlb1->TLBID < tlb2->TLBID;
	}

	bool condicion_busqueda(Tlb* tlb){
		if (tlb->TLBID == TlbID){
			TlbID++;
			return false;
		}
		else{
			return true;
		}
	}

	if (list_size(tlbs->lista)==0){
		//Si esta vacia devuelvo tlb 0
		return TlbID;
	}
	else{
		//Ordeno por TlbID
		list_sort(tlbs->lista,(void*)condicion_sort);

		//Recorro la lista hasta que encuentre mi TlbID Libre
		Tlb* tlb_aux = list_find(tlbs->lista,(void*)condicion_busqueda);
		return TlbID;
	}
}

void agregar_tlb(Tlbs* tlbs, Tlb* tlb){
	bool condicion_sort(Tlb* entrada1,Tlb* entrada2){
		return entrada1->REFERENCIADA < entrada2->REFERENCIADA;
	}

	Tlb* tlb_aux = buscar_tlb(tlbs,tlb->NRO_PAGINA,tlb->PID);
	if(tlb_aux == NULL){ //significa que no estaba, entonces la agrego
		if(list_size(tlbs->lista) < tlbs->CANT_ENTRADAS){
			//Tengo espacio en la TLB
			int TlbID = conseguir_id_tlb_libre(tlbs);
			tlb->TLBID = TlbID;
			list_add(tlbs->lista,tlb);
		}
		else{
			//No tengo espacio, aplico LRU
			//Ordeno por Referencia
			list_sort(tlbs->lista,(void*)condicion_sort);

			//Lo coloco en el primer lugar (ya esta ordenada)
			Tlb* tlb_reemplazo = list_get(tlbs->lista,0);
			tlb_reemplazo->PID = tlb->PID;
			tlb_reemplazo->NRO_PAGINA = tlb->NRO_PAGINA;
			tlb_reemplazo->PRESENTE = tlb->PRESENTE;
			tlb_reemplazo->REFERENCIADA = tlb->REFERENCIADA;
			//list_replace_and_destroy_element(tlbs->lista,0,tlb,(void*)destroy_tlb);
		}
	}
	else{
		tlb_aux->REFERENCIADA++;
	}
}

Tlb* buscar_tlb(Tlbs* tlbs, int NRO_PAG, int PID){
	bool condicion_busqueda(Tlb* tlb){
		return tlb->NRO_PAGINA == NRO_PAG && tlb->PID == PID;
	}

	return list_find(tlbs->lista,(void*)condicion_busqueda);
}

void borrar_tlb_completa(Tlbs* tlbs){
	tlbs->TLBID = -1;
	list_clean_and_destroy_elements(tlbs->lista,(void*)destroy_tlb);
}

void borrar_tlb_x_pid(Tlbs* tlbs, int PID){
	bool condicion_busqueda(Tlb* tlb){
		return tlb->PID == PID;
	}

	int cantidad_a_borrar = list_count_satisfying(tlbs->lista,(void*)condicion_busqueda);
	int index;
	for(index = 0;index <= cantidad_a_borrar; index++){
		list_remove_and_destroy_by_condition(tlbs->lista,(void*)condicion_busqueda,(void*)destroy_tlb);
	}
}

//Funcion utilizada para cadena "PID1, PID2, etc"
char* recorrer_tabla_tlbs(Tlbs* tlbs){
	pthread_mutex_lock(&tlbs->sem_tlb_visor); //varios hilos muestran el visor
	char* cadena_PID = string_new();

	void funcion_closure(Tlb* tlb){
		char* TLBID = string_itoa(tlb->TLBID);
		char* PID = string_itoa(tlb->PID);
		char* NRO_PAG = string_itoa(tlb->NRO_PAGINA);
		char* NRO_MARCO = string_itoa(tlb->NRO_MARCO);
		char* REFERENCIADA = string_itoa(tlb->REFERENCIADA);
		string_append(&cadena_PID,TLBID);
		string_append(&cadena_PID,"           ");
		string_append(&cadena_PID,"PID");
		string_append(&cadena_PID,PID);
		string_append(&cadena_PID,"     ");
		string_append(&cadena_PID,NRO_PAG);
		string_append(&cadena_PID,"               ");
		string_append(&cadena_PID,NRO_MARCO);
		string_append(&cadena_PID,"             ");
		string_append(&cadena_PID,REFERENCIADA);
		string_append(&cadena_PID,"\n");
	}

	list_iterate(tlbs->lista,(void*)funcion_closure);

	pthread_mutex_unlock(&tlbs->sem_tlb_visor); //varios hilos muestran el visor
	return cadena_PID;
}

//Funcion utilizada para cadena "PID1"
char* recorrer_tabla_tlbs_un_pid(Tlbs* tlbs,int PID){
	pthread_mutex_lock(&tlbs->sem_tlb_visor); //varios hilos muestran el visor
	char* cadena_PID = string_new();

	void funcion_closure(Tlb* tlb){
		if(tlb->PID == PID){
			char* TLBID = string_itoa(tlb->TLBID);
			char* PID = string_itoa(tlb->PID);
			char* NRO_PAG = string_itoa(tlb->NRO_PAGINA);
			char* NRO_MARCO = string_itoa(tlb->NRO_MARCO);
			char* REFERENCIADA = string_itoa(tlb->REFERENCIADA);
			string_append(&cadena_PID,TLBID);
			string_append(&cadena_PID,"           ");
			string_append(&cadena_PID,"PID");
			string_append(&cadena_PID,PID);
			string_append(&cadena_PID,"     ");
			string_append(&cadena_PID,NRO_PAG);
			string_append(&cadena_PID,"               ");
			string_append(&cadena_PID,NRO_MARCO);
			string_append(&cadena_PID,"             ");
			string_append(&cadena_PID,REFERENCIADA);
			string_append(&cadena_PID,"\n");
		}
	}

	list_iterate(tlbs->lista,(void*)funcion_closure);

	pthread_mutex_unlock(&tlbs->sem_tlb_visor); //varios hilos muestran el visor
	return cadena_PID;
}
