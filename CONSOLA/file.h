#ifndef CONSOLA2_FILE_H_
#define CONSOLA2_FILE_H_

#include <stdio.h>

typedef struct archivo Archivo;

struct archivo {
	FILE* file;
	char* path;
	char* modo;
	int size_file;
};

Archivo* new_archivo();

char* leer_archivo(Archivo* archivo);

#endif /* CONSOLA2_FILE_H_ */
