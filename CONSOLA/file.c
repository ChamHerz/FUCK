#include "file.h"
#include <stdio.h>
#include <commons/string.h>

Archivo* new_archivo(){
	Archivo* a;
	a = malloc(sizeof(struct archivo));
	return a;
}

void destroy_archivo(Archivo* archivo){
	free(archivo);
}

char* leer_archivo(Archivo* archivo){
	FILE* file = archivo->file;
	char caracteres[100];
	file = fopen(archivo->path,archivo->modo);

	//printf("\nEl archivo contine lo siguiente:\n");

	char* cadena_file = string_new();

	while(feof(file)==0){
		fgets(caracteres,100,file);
		string_append(&cadena_file,caracteres);
	}

	archivo->size_file = string_length(cadena_file);

	fclose(file);

	return cadena_file;
}
