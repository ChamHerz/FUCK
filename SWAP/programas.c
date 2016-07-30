#include "programas.h"

void agregar_programa(Programas* programas, Programa* programa){
	list_add(programas->lista,programa);
}

Programa* buscar_programa(Programas* programas, int PID){
	bool condicion_busqueda(Programa* programa){
		return programa->PID == PID;
	}

	return list_find(programas->lista,(void*)condicion_busqueda);
}

//constructor
Programas* new_programas(){
	Programas* p;
	p = malloc(sizeof(struct programas));
	p->lista = list_create();
	return p;
}

//Destructor
void destroy_programas(Programas* programas){
	list_destroy(programas->lista);
	free(programas);
}

void borrar_programa(Programas* programas,int PID){
	bool condicion_busqueda(Programa* programa){
		return programa->PID == PID;
	}

	return list_remove_and_destroy_by_condition(programas->lista,(void*)condicion_busqueda,(void*)destroy_programa);
}

//Obtiene la cantidad de paginas usadas en todo el swap
int total_paginas_usadas(Programas* programas){
	int total = 0;

	void funcion_closure(Programa* programa){
		total = total + programa->CANT_PAG;
	}

	list_iterate(programas->lista,(void*)funcion_closure);
	return total;
}

//Funcion utilizada para cadena "PID1, PID2, etc"
char* recorrer_tabla_programas(Programas* programas){
	char* cadena_PID = string_new();

	void funcion_closure(Programa* programa){
		char* PID = string_itoa(programa->PID);
		char* SIZE = string_itoa(programa->SIZE);
		char* SIZE_PAG = string_itoa(programa->SIZE_PAGE);
		char* CANT_PAG = string_itoa(programa->CANT_PAG);


		string_append(&cadena_PID,"PID");
		string_append(&cadena_PID,PID);
		string_append(&cadena_PID,"     ");
		string_append(&cadena_PID,SIZE);
		string_append(&cadena_PID,"             ");
		string_append(&cadena_PID,SIZE_PAG);
		string_append(&cadena_PID,"           ");
		string_append(&cadena_PID,CANT_PAG);

		string_append(&cadena_PID,"\n");
	}

	list_iterate(programas->lista,(void*)funcion_closure);

	return cadena_PID;
}
