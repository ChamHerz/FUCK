#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <pthread.h>

typedef struct variable Variable;

struct variable {
	//PROPIEDADES
	char* id_variable;
	int valor_variable;

	//MUTEX
	pthread_mutex_t MUTEX;
};
//Constructor
Variable* new_variable(char* id_variable,int valor_variable);

//Destructor
void destroy_variable(Variable* variable);

#endif /* VARIABLE_H_ */
