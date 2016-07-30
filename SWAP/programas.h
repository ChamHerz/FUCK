#ifndef PROGRAMAS_H_
#define PROGRAMAS_H_

#include <commons/collections/list.h>
#include <commons/string.h>
#include "programa.h"

typedef struct programas Programas;

struct programas {
	//PROPIEDADES
	int pag_totales;
	t_list* lista;
};
//Metodos
void agregar_programa(Programas* programas, Programa* programa);
Programa* buscar_programa(Programas* programas, int PID);
char* recorrer_tabla_programas(Programas* programas);
void borrar_programa(Programas* programas,int PID);
int total_paginas_usadas(Programas* programas);

//Constructor
Programas* new_programas();

//Destructor
void destroy_programas(Programas* programas);

#endif /* PROGRAMAS_H_ */
