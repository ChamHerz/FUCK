#include "pcbs.h"

//ESTRUCTURAS PARA BLOQUEADOS
Pidblock* new_pidblock(int PID,char SEMAFORO){
	Pidblock* p;
	p = malloc(sizeof(struct pidblock));
	p->PID = PID;
	p->SEMAFORO = SEMAFORO;
	return p;
}

Pidblock* new_pidblock_io(int PID,char* IO, int CANTIDAD_IO){
	Pidblock* p;
	p = malloc(sizeof(struct pidblock));
	p->PID = PID;
	p->IO = IO;
	p->CANTIDAD_IO = CANTIDAD_IO;
	return p;
}

Pidblocks* new_pidblocks(){
	Pidblocks* p;
	p = malloc(sizeof(struct pidblocks));
	p->lista = list_create();
	return p;
}

void agregar_pidblock(Pidblocks* pidblocks, Pidblock* pidblock){
	list_add(pidblocks->lista,pidblock);
}

Pidblock* buscar_y_sacar_pidlock(Pidblocks* pidblocks,int pid){
	bool condicion_busqueda(Pidblock* pidblock){
		return pidblock->PID == pid;
	}

	return list_remove_by_condition(pidblocks->lista,(void*)condicion_busqueda);
}

Pidblock* buscar_y_sacar_pidblock_semaforo(Pidblocks* pidblocks,char semaforo){
	bool condicion_busqueda(Pidblock* pidblock){
		return pidblock->SEMAFORO == semaforo;
	}

	return list_remove_by_condition(pidblocks->lista,(void*)condicion_busqueda);
}

Pidblock* buscar_y_sacar_pidblock_io(Pidblocks* pidblocks,int PID){
	bool condicion_busqueda(Pidblock* pidblock){
		return pidblock->PID == PID;
	}

	return list_remove_by_condition(pidblocks->lista,(void*)condicion_busqueda);
}

//ESTRUCTURAS PARA BLOQUEADOS

Pcbs* new_pcbs(){
	Pcbs* p;
	p = malloc(sizeof(struct pcbs));
	p->lista = list_create();
	return p;
}

void agregar_pcb(Pcbs* pcbs, Pcb* pcb){
	list_add(pcbs->lista,pcb);
}

int esta_vacia_la_cola(Pcbs* pcbs){
	return list_is_empty(pcbs->lista);
}

void vaciar_cola(Pcbs* pcbs){
	usleep(60000000);	//despues de 1 minutos vacia la cola de terminados
	list_clean_and_destroy_elements(pcbs->lista,(void*)destroy_pcb);
}

Pcb* sacar_cabeza(Pcbs* pcbs){
	return list_remove(pcbs->lista,0);
}

Pcb* get_cabeza(Pcbs* pcbs){
	return list_get(pcbs->lista,0);
}

Pcb* buscar_pcb(Pcbs* pcbs,int PID){
	bool condicion_busqueda(Pcb* pcb){
		return pcb->PID == PID;
	}

	return list_find(pcbs->lista,(void*)condicion_busqueda);
}

Pcb* buscar_pcb_bloqueado_semaforo(Pcbs* pcbs,int motivo_bloqueo,char id_semaforo){
	bool condicion_busqueda(Pcb* pcb){
		return pcb->ESTADO_FINAL == BLOQUEO_WAIT && pcb->SEMAFORO == id_semaforo;
	}

	return list_find(pcbs->lista,(void*)condicion_busqueda);
}

Pcb* sacar_pcb(Pcbs* pcbs,int PID){
	bool condicion_busqueda(Pcb* pcb){
		return pcb->PID == PID;
	}

	return list_remove_by_condition(pcbs->lista,(void*)condicion_busqueda);
}

//Funcion utilizada para cadena "PID1, PID2, etc"
char* recorrer_cola(Pcbs* pcbs){
	char* cadena_PID = string_new();

	void funcion_closure(Pcb* pcb){
		char* PID = string_itoa(pcb->PID);
		string_append(&cadena_PID," PID");
		string_append(&cadena_PID,PID);
	}

	list_iterate(pcbs->lista,(void*)funcion_closure);

	return cadena_PID;
}
