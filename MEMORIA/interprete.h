/*
 * interprete.h
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#ifndef INTERPRETE_H_
#define INTERPRETE_H_

#include <commons/collections/dictionary.h>

#define PROMPT "#@> "
#define MAX_CADENA 50

typedef struct interprete Interprete;

struct interprete {
	//PROPIEDADES
	t_dictionary* comandos;
	int comando_correcto;
	char** commandos_ingresados;
};

Interprete* new_interprete();

void destroy_interprete(Interprete* this);

int iniciar_interprete(Interprete* interprete);

int controlar_cadena(char* cadena, Interprete* interprete);

int buscar_comando(char* comando,Interprete* interprete);

#endif /* INTERPRETE_H_ */
