#include "file.h"
#include <stdio.h>
#include <commons/string.h>

Archivo* new_archivo(){
	Archivo* a;
	a = malloc(sizeof(struct archivo));
	return a;
}

char* leer_archivo(char* path,int start_pag,Direccion* direccion,int size_pag){
	FILE* file;
	char* caracteres;
	//caracteres = malloc(size_pag);
	caracteres = calloc(size_pag,sizeof(char));
	file = fopen(path,"rb+");

	printf("\nEl archivo contine lo siguiente:\n");

	int start = start_pag * size_pag;
	start = start + (direccion->PAG * size_pag);

	fseek(file,start,SEEK_SET);

	printf("\nSe lee: pid %d start %d size %d\n",direccion->PID,start,size_pag);
	int leidos = fread(caracteres,sizeof(char),size_pag,file);

	printf("\nSe leyeron %d bytes\n",leidos);

	fclose(file);

	return caracteres;
}

void escribir_archivo(char* path,int start_pag,Direccion* direccion,int size_pag,char* pagina){
	FILE* file;
	file = fopen(path,"rb+");

	int start = start_pag * size_pag;
	start = start + (direccion->PAG * size_pag);

	fseek(file,start,SEEK_SET);

	printf("\nSe escribe: pid %d start %d size %d\n",direccion->PID,start,size_pag);
	int escritos = fwrite(pagina,sizeof(char),size_pag,file);

	printf("\nSe escribieron %d bytes\n",escritos);

	fclose(file);
}

void escribir_swap(char* path,Ansisop* ansisop,Programa* programa){
	int start_pag = programa->START_PAG;
	int start_byte = start_pag * programa->SIZE_PAGE;
	printf("Se guardo el codigo desde el Byte: %d\n",start_byte);
	FILE* file;
	file = fopen(path,"rb+");
	fseek(file,start_byte,SEEK_SET);
	//fputs(ansisop->CADENA,file);
	fwrite(ansisop->CADENA,1,programa->SIZE,file);

	fclose(file);
}

void borrar_ansisop_del_swap(char* path,Programa* programa){
	int start_pag = programa->START_PAG;
	int start_byte = start_pag * programa->SIZE_PAGE;

	FILE* file;
	file = fopen(path,"a");
	fseek(file,start_byte,SEEK_SET);

	char* cadena = string_repeat('\0',programa->SIZE);
	fwrite(cadena,1,programa->SIZE,file);

	fclose(file);
	printf("Se borro el codigo desde el Byte: %d la cantidad de %d\n",start_byte,programa->SIZE);
}
