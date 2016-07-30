#ifndef MARCOS_H_
#define MARCOS_H_

#include "marco.h"
#include <commons/collections/list.h>
#include <pthread.h>

typedef struct marcos Marcos;

struct marcos {
	//PROPIEDADES
	int MARCOID_ACTUAL;
	t_list* lista;
};

Marcos* new_marcos();

void destroy_marcos(Marcos* marcos);

int cantidad_marcos_ocupados(Marcos* marcos);

void agregar_marco(Marcos* marcos, Marco* marco);

Marco* buscar_marco(Marcos* marcos, int PID, int pagina);

char* recorrer_tabla_marcos(Marcos* marcos);

void borrar_marco(Marcos* marcos,int PID);

void set_bit_usado_marco(Marcos* marcos, int id_marco);

Marco* buscar_marco_idMarco(Marcos* marcos, int MarcoID);

Marco* buscar_marco_reemplaza_clock(Marcos* marcos,int PUNTERO_CLOCK);

Marco* buscar_marco_reemplazo_cambiar_usados(Marcos* marcos,int PUNTERO_CLOCK);

Marco* buscar_usado_modificado_cambia(Marcos* marcos,int PUNTERO_CLOCK);

Marco* solo_buscar_usado_modificado(Marcos* marcos,int PUNTERO_CLOCK);

int obtener_index(Marcos* marcos,int IDMARCO);

void pasar_todas_modificado(Marcos* marcos);

#endif /* MARCOS_H_ */
