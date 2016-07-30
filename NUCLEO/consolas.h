#include "consola.h"
#include <commons/collections/list.h>

#ifndef CONSOLAS_H_
#define CONSOLAS_H_

typedef struct consolas Consolas;

struct consolas {
	//PROPIEDADES
	t_list* lista;
	int id_consola;
};
//Metodos
int esta_ocupada(Consolas* consolas,int descriptor);
Consola* buscar_consola(Consolas* consolas, int descriptor);
void set_accion(Consolas* consolas,int descriptor, int accion);
void agregar_consola(Consolas* consolas, Consola* consola);
void borrar_consola(Consolas* consolas, int id_consola);
int cantidad_consolas(Consolas* consolas);

//Constructor
Consolas * new_consolas();

//Destructor
void destroy_consolas(Consolas * this);

#endif /* CONSOLAS_H_ */
