#include "util.h"
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int cantidad_paginas_necesarias(int cant_byte,int pag_size){
	int cant_pag;
	cant_pag = cant_byte / pag_size;
	if (cant_byte % pag_size != 0){
		return cant_pag + 1;
	}
	return cant_pag;
}

int convertir_cadena_a_entero(char* cad){
	int num = 0;
	int size = string_length(cad);
	int digito;
	int i;

	for( i = size; i >0; --i){
		digito = (int)(cad[i-1] - 48);
		num += digito * pow(10.0, size - i);
	}

	return num;
}

void string_convert_array(char** vector,char* cadena){
	int i = 1;
	string_append(&cadena,"[");
	string_append(&cadena,vector[0]);
	while(vector[i] != NULL){
		string_append(&cadena,",");
		string_append(&cadena,vector[i]);
		i = i + 1;
	}
	string_append(&cadena,"]");
	printf("Cadena: %s \n", cadena);
}

void config_get_array_value_int(char** vector,int* vector_int){
	int i = 0;
	int j = 0;
	int vector_size = 0;
	int digito;
	int numero = 0;
	int size;
	while(vector[vector_size] != NULL){
		vector_size += 1;
	}
	vector_int = (int*)malloc(vector_size * sizeof(int));
	while(vector[i] != NULL){
		size = string_length(vector[i]);
		for(j = size; j > 0; --j){
			digito = (int)(vector[i][j-1] - 48);
			numero += digito * pow(10.0, size -j);
			vector_int[i] = numero;
		}
		i = i + 1;
	}
}
