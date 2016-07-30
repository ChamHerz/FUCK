#ifndef PCBS_H_
#define PCBS_H_

#include "pcb.h"
#include <commons/collections/list.h>

#define BLOQUEO_WAIT 1
#define FINAL_PROGRAMA 2
#define QUANTUM_COMPLETO 3
#define BLOQUEO_IO 4

typedef struct pcbs Pcbs;

struct pcbs {
	//PROPIEDADES
	t_list* lista;
};

//ESTRUCTURA PARA BLOQUEADOS
typedef struct pidblocks Pidblocks;

struct pidblocks{
	t_list* lista;
};

typedef struct pidblock Pidblock;

struct pidblock{
	int PID;
	char SEMAFORO;
	char* IO;
	int CANTIDAD_IO;
};

Pidblock* new_pidblock(int PID,char SEMAFORO);

Pidblock* new_pidblock_io(int PID,char* IO, int CANTIDAD_IO);

Pidblocks* new_pidblocks();

Pidblock* buscar_y_sacar_pidblock_semaforo(Pidblocks* pidblocks,char semaforo);

Pidblock* buscar_y_sacar_pidblock_io(Pidblocks* pidblocks,int PID);

//ESTRUCTURA PARA BLOQUEADOS

Pcbs* new_pcbs();

void agregar_pcb(Pcbs* pcbs, Pcb* pcb);

int esta_vacia_la_cola(Pcbs* pcbs);

Pcb* sacar_cabeza(Pcbs* pcbs);

Pcb* buscar_pcb(Pcbs* pcbs,int PID);

Pcb* sacar_pcb(Pcbs* pcbs,int PID);

char* recorrer_cola(Pcbs* pcbs);

void vaciar_cola(Pcbs* pcbs);

Pcb* buscar_pcb_bloqueado_semaforo(Pcbs* pcbs,int motivo_bloqueo,char id_semaforo);

#endif /* PCBS_H_ */
