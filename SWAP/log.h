#ifndef LOG_H_
#define LOG_H_

#include "programas.h"
#include "bitmap.h"
#include <stdio.h>

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

void mostrar_estado_swap(Log* log, Programas* programas, Bitmap* bitmap);

void mostrar_estado_paginas(Log* log,int pag_totales, int pag_actuales);

#endif /* LOG_H_ */
