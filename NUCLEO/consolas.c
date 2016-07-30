#include "consolas.h"

//Metodos
int esta_ocupada(Consolas* consolas,int descriptor){
	Consola* consola = buscar_consola(consolas,descriptor);
	int estado = consola->ocupada;
	if(consola->ocupada == 0){
		consola->ocupada = 1;
	}
	return estado;
}

void set_accion(Consolas* consolas,int descriptor, int accion){
	Consola* consola = buscar_consola(consolas,descriptor);
	consola->accion = accion;
}

Consola* buscar_consola(Consolas* consolas, int descriptor){
	bool condicion_busqueda(Consola* consola){
		return consola->descriptor == descriptor;
	}

	return list_find(consolas->lista,(void*)condicion_busqueda);
}

Consola* buscar_consola_pid(Consolas* consolas, int pid){
	bool condicion_busqueda(Consola* consola){
		return consola->PID == pid;
	}

	return list_find(consolas->lista,(void*)condicion_busqueda);
}

void agregar_consola(Consolas* consolas, Consola* consola){
	consolas->id_consola++;
	consola->id_consola = consolas->id_consola;
	list_add(consolas->lista,consola);
}

void borrar_consola(Consolas* consolas, int id_consola){
	bool condicion_borrado(Consola* consola){
		return consola->id_consola == id_consola;
	}

	Consola* consola;
	consola = list_remove_by_condition(consolas->lista,(void*)condicion_borrado);
}

int cantidad_consolas(Consolas* consolas){
	return list_size(consolas->lista);
}

//constructor
Consolas * new_consolas(){
	Consolas * c;
	c = malloc(sizeof(struct consolas));
	c->lista = list_create();
	c->id_consola = -1;
	return c;
}

//Destructor
void destroy_consolas(Consolas * this){
	free(this);
}
