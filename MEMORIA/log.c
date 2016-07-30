#include "log.h"
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

void save_log_char(Log* log,char* formato,char* cadena){
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,formato,cadena);
	fclose(log->FILE);
}

void save_log_int(Log* log,char* formato,int variable){
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,formato,variable);
	fclose(log->FILE);
}

void save_log_int2(Log* log,char* formato,int var1,int var2){
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,formato,var1,var2);
	fclose(log->FILE);
}

void mostrar_estado_tlbs(Log* log,Tlbs* tlbs){
	save_log(log,"TABLA ESTADO TLB:");
	save_log(log,"=================");
	save_log(log,"NRO TLB     PID      NRO PAGINA      NRO MARCO     REFERENCIADA");
	char* procesosid = recorrer_tabla_tlbs(tlbs);
	save_log(log,procesosid);
}

void mostrar_estado_programas(Log* log,Programas* programas){
	save_log(log,"TABLA ESTADO PROGRAMAS:");
	save_log(log,"======================");
	save_log(log,"PID      CANT. MARCOS");
	char* procesosid = recorrer_tabla_programas(programas);
	save_log(log,procesosid);
}

void mostrar_estado_marcos(Log* log,Marcos* marcos){
	save_log(log,"TABLA ESTADO MARCOS:");
	save_log(log,"======================");
	save_log(log,"NRO MARCO     PID      NRO PAGINA      USADA      MODIFICADA");
	char* procesosid = recorrer_tabla_marcos(marcos);
	save_log(log,procesosid);
}

void mostrar_estado_memoria(Log* log,Tlbs* tlbs,Programas* programas,Marcos* marcos){
	inicio_log(log);
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,"%s\n","TABLA ESTADO TLB:\n==================\nNRO TLB     PID      NRO PAGINA      NRO MARCO     REFERENCIADA");
	//fprintf(log->FILE,"%s\n","==================");
	//fprintf(log->FILE,"%s\n","NRO TLB     PID      NRO PAGINA      NRO MARCO     REFERENCIADA");
	fprintf(log->FILE,"%s\n",recorrer_tabla_tlbs(tlbs));
	fprintf(log->FILE,"%s\n","TABLA ESTADO PROGRAMAS:\n=======================\nPID      CANT. MARCOS");
	//fprintf(log->FILE,"%s\n","=======================");
	//fprintf(log->FILE,"%s\n","PID      CANT. MARCOS");
	fprintf(log->FILE,"%s\n",recorrer_tabla_programas(programas));
	fprintf(log->FILE,"%s\n","TABLA ESTADO MARCOS:\n====================\nNRO MARCO     PID      NRO PAGINA      USADA      MODIFICADA");
	//fprintf(log->FILE,"%s\n","====================");
	//fprintf(log->FILE,"%s\n","NRO MARCO     PID      NRO PAGINA      USADA      MODIFICADA");
	fprintf(log->FILE,"%s\n",recorrer_tabla_marcos(marcos));
	fclose(log->FILE);
}

void mostrar_estado_memoria_un_pid(Log* log,Tlbs* tlbs,Programas* programas,Marcos* marcos,int PID){
	inicio_log(log);
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,"%s\n","TABLA ESTADO TLB:\n==================\nNRO TLB     PID      NRO PAGINA      NRO MARCO     REFERENCIADA");
	//fprintf(log->FILE,"%s\n","==================");
	//fprintf(log->FILE,"%s\n","NRO TLB     PID      NRO PAGINA      NRO MARCO     REFERENCIADA");
	fprintf(log->FILE,"%s\n",recorrer_tabla_tlbs_un_pid(tlbs,PID));
	fprintf(log->FILE,"%s\n","TABLA ESTADO PROGRAMAS:\n=======================\nPID      CANT. MARCOS");
	//fprintf(log->FILE,"%s\n","=======================");
	//fprintf(log->FILE,"%s\n","PID      CANT. MARCOS");
	fprintf(log->FILE,"%s\n",recorrer_tabla_programas_un_pid(programas,PID));
	fprintf(log->FILE,"%s\n","TABLA ESTADO MARCOS:\n====================\nNRO MARCO     PID      NRO PAGINA      USADA      MODIFICADA");
	//fprintf(log->FILE,"%s\n","====================");
	//fprintf(log->FILE,"%s\n","NRO MARCO     PID      NRO PAGINA      USADA      MODIFICADA");
	fprintf(log->FILE,"%s\n",recorrer_tabla_marcos_un_pid(marcos,PID));
	fclose(log->FILE);
}

void mostrar_contenido_all(Log* log,Marcos* marcos,char* vector){
	inicio_log(log);
	int index;
	for(index = 0; index < list_size(marcos->lista); index++){
		fprintf(log->FILE,"\nId Marco: %d Contenido: %s\n",index,vector[index]);
	}
	fclose(log->FILE);
}
