#ifndef PROGRAMAS_H_
#define PROGRAMAS_H_

#include "programa.h"
#include <commons/string.h>
#include <commons/collections/list.h>

typedef struct programas Programas;

struct programas {
	//PROPIEDADES
	t_list* lista;
};

Programas* new_programas();

void destroy_programas(Programas* programas);

int cantidad_programas(Programas* programas);

Programa* buscar_programa(Programas* programa, int PID);

void agregar_programa(Programas* programas, Programa* programa);

void borrar_programa(Programas* programas,int PID);

char* recorrer_tabla_programas(Programas* programas);

#endif /* PROGRAMAS_H_ */
