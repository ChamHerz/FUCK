#ifndef FILE_H_
#define FILE_H_

#include "ansisop.h"
#include "direccion.h"
#include "programa.h"
#include <stdio.h>

typedef struct archivo Archivo;

struct archivo {
	FILE* file;
	char* path;
	char* modo;
};

Archivo* new_archivo();

char* leer_archivo(char* path,int start_pag,Direccion* direccion,int size_pag);

//void escribir_archivo(char* path,Ansisop* ansisop);

void escribir_swap(char* path,Ansisop* ansisop,Programa* programa);

#endif /* FILE_H_ */
