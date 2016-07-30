#include "variable.h"
#include <stdlib.h>

//constructor
Variable* new_variable(char* id_variable,int valor_variable){
	Variable* v;
	v = malloc(sizeof(struct variable));
	v->id_variable = id_variable;
	v->valor_variable = valor_variable;
	//MUTEX
	pthread_mutex_init(&v->MUTEX, NULL);
	return v;
}

//Destructor
void destroy_variable(Variable* variable){
	free(variable);
}
