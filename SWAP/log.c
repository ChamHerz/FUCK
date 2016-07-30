#include "log.h"
#include "bitmap.h"
#include <stdlib.h>

//constructor
Log* new_log(char* PATH){
	Log* l;
	l = malloc(sizeof(struct log));
	//Propiedades
	l->PATH = PATH;
	inicio_log(l);
	return l;
}

void inicio_log(Log* log){
	log->FILE = fopen(log->PATH,"w");
	fflush(log->FILE);
	fclose(log->FILE);
}

void save_log(Log* log,const char* cadena){
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,"%s\n",cadena);
	fclose(log->FILE);
}

void save_log_int(Log* log,char* formato,int variable){
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,formato,variable);
	fclose(log->FILE);
}

void save_log_pag(Log* log,char* formato,int num1,int num2){
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,formato,num1,num2);
	fclose(log->FILE);
}

void mostrar_estado_swap(Log* log, Programas* programas, Bitmap* bitmap){
	inicio_log(log);
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,"%s\n","ESTADO DE SWAP:\n==============\nPID      SIZE(BYTES)     SIZE PAG     CANT PAG");
	fprintf(log->FILE,"%s\n",recorrer_tabla_programas(programas));
	fprintf(log->FILE,"PAG. TOTALES: %d USADAS: %d\n",bitmap->cant_paginas,total_paginas_usadas(programas));
	fprintf(log->FILE,"%s\n",mostrar_mapa(bitmap));
	fclose(log->FILE);
}

void mostrar_estado_paginas(Log* log,int pag_totales, int pag_actuales){
	save_log_pag(log,"PAG. TOTALES: %d USADAS: %d",pag_totales,pag_actuales);
}
