#include "variables.h"
#include <stdlib.h>
//constructor
Variables* new_variables(){
	Variables* v;
	v = malloc(sizeof(struct variables));
	v->lista = list_create();
	return v;
}

//Destructor
void destroy_variables(Variables* variables){
	free(variables);
}

void agregar_variable(Variables* variables, Variable* variable){
	list_add(variables->lista,variable);
}

Variable* buscar_variable(Variables* variables, char* id_variable){
	//	IMPORTANTE !!!!!! ************

	// TUVIMOS QUE DESARROLLAR NUESTRA PROPIA FUNCION PARA COMPARAR DOS STRING....
	// PORQUE LA FUNCION string_equals_ignore_case DE LA commons library
	// presentaba el error de devolver TRUE cuando se le pasa la cadena "colas" y la cadena "compartida"
	// se desconoce el error. AVISAR A LOS TUTORES SOBRE ESTE BUG!!!


	bool comparar_cadenas(char* cad1, char* cad2){
		bool resultado;
		if(strcmp(cad1,cad2) == 0){
			resultado = true;
		}
		else{
			resultado = false;
		}
		return resultado;
	}

	bool condicion_busqueda(Variable* variable){
		//char* cadena1 = string_new();
		//char* cadena2 = string_new();
		//string_append(&variable->id_variable,"\0");
		//string_append(&id_variable,"\0");
		//char* cadena1 = string_duplicate(variable->id_variable); //colas
		//char* cadena2 = string_duplicate(id_variable); //compartida
		//bool bool_aux = string_equals_ignore_case(variable->id_variable,id_variable);
		//free(cadena1);
		//free(cadena2);
		bool bool_aux = comparar_cadenas(variable->id_variable,id_variable);

		return bool_aux;
	}

	return list_find(variables->lista,(void*)condicion_busqueda);
}
