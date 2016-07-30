#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "variable.h"
#include <commons/collections/list.h>

typedef struct variables Variables;

struct variables {
	//PROPIEDADES
	t_list* lista;
};

Variables* new_variables();

void destroy_variables(Variables* variables);

void agregar_variable(Variables* variables, Variable* variable);

Variable* buscar_variable(Variables* variables, char* id_variable);

#endif /* VARIABLES_H_ */
