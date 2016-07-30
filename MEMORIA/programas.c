#include "programas.h"
#include <stdlib.h>

//constructor
Programas* new_programas(){
	Programas* p;
	p = malloc(sizeof(struct programas));
	p->lista = list_create();
	return p;
}

//Destructor
void destroy_programas(Programas* programas){
	free(programas);
}

int cantidad_programas(Programas* programas){
	return list_size(programas->lista);
}

Programa* buscar_programa(Programas* programas, int PID){
	bool condicion_busqueda(Programa* programa){
		return programa->PID == PID;
	}

	return list_find(programas->lista,(void*)condicion_busqueda);
}

void agregar_programa(Programas* programas, Programa* programa){
	list_add(programas->lista,programa);
}

Programa* buscar_y_sacar_programa(Programas* programas, int PID){
	bool condicion_busqueda(Programa* programa){
		return programa->PID == PID;
	}

	return list_remove_by_condition(programas->lista,(void*)condicion_busqueda);
}

void borrar_programa(Programas* programas,int PID){
	bool condicion_busqueda(Programa* programa){
		return programa->PID == PID;
	}

	return list_remove_and_destroy_by_condition(programas->lista,(void*)condicion_busqueda,(void*)destroy_programa);
}

//Funcion utilizada para cadena "PID1, PID2, etc"
char* recorrer_tabla_programas(Programas* programas){
	char* cadena_PID = string_new();

	void funcion_closure(Programa* programa){
		char* PID = string_itoa(programa->PID);
		char* CANT_MARCOS = string_itoa(programa->CANT_MARCOS);
		string_append(&cadena_PID,"PID");
		string_append(&cadena_PID,PID);
		string_append(&cadena_PID,"     ");
		string_append(&cadena_PID,CANT_MARCOS);
		string_append(&cadena_PID,"\n");
	}

	list_iterate(programas->lista,(void*)funcion_closure);

	return cadena_PID;
}

//Funcion utilizada para cadena "PID1"
char* recorrer_tabla_programas_un_pid(Programas* programas,int PID){
	char* cadena_PID = string_new();

	void funcion_closure(Programa* programa){
		if (programa->PID == PID){
			char* PID = string_itoa(programa->PID);
			char* CANT_MARCOS = string_itoa(programa->CANT_MARCOS);
			string_append(&cadena_PID,"PID");
			string_append(&cadena_PID,PID);
			string_append(&cadena_PID,"     ");
			string_append(&cadena_PID,CANT_MARCOS);
			string_append(&cadena_PID,"\n");
		}
	}

	list_iterate(programas->lista,(void*)funcion_closure);

	return cadena_PID;
}
