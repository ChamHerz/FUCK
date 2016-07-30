#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include "programas.h"
#include "marcos.h"
#include "tlbs.h"

typedef struct log Log;

struct log {
	//PROPIEDADES
	FILE* FILE;
	char* PATH;
};

//CONSTRUCTOR
Log* new_log(char* PATH);

void inicio_log(Log* log);

void save_log(Log* log,const char* cadena);

void save_log_int(Log* log,char* formato,int variable);

void mostrar_estado_tlbs(Log* log,Tlbs* tlbs);

void mostrar_estado_programas(Log* log,Programas* programas);

void mostrar_estado_marcos(Log* log,Marcos* marcos);

#endif /* LOG_H_ */
