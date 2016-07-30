#include "log.h"
#include <stdio.h>

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

void mostrar_estado_nuevos(Log* log, Pcbs* pcbs){
	save_log(log,"COLA NUEVOS:");
	save_log(log,"============");
	char* procesosid = recorrer_cola(pcbs);
	save_log(log,procesosid);
}

void mostrar_estado_listos(Log* log, Pcbs* pcbs){
	save_log(log,"COLA LISTOS:");
	save_log(log,"============");
	char* procesosid = recorrer_cola(pcbs);
	save_log(log,procesosid);
}

void mostrar_estado_ejecutando(Log* log, Pcbs* pcbs){
	save_log(log,"COLA EJECUTANDO:");
	save_log(log,"================");
	char* procesosid = recorrer_cola(pcbs);
	save_log(log,procesosid);
}

void mostrar_estado_bloqueados(Log* log, Pcbs* pcbs){
	save_log(log,"COLA BLOQUEADOS:");
	save_log(log,"===============");
	char* procesosid = recorrer_cola(pcbs);
	save_log(log,procesosid);
}

void mostrar_estado_finalizados(Log* log, Pcbs* pcbs){
	save_log(log,"COLA TERMINADOS:");
	save_log(log,"===============");
	char* procesosid = recorrer_cola(pcbs);
	save_log(log,procesosid);
}

void mostrar_estado_cpus(Log* log, Cpus* cpus){
	save_log(log,"ESTADO CPUS:");
	save_log(log,"============");
	save_log(log,"DESCR  OCUPADO  OSIOSA    PID    ID_PROCESS");
	char* procesosid = recorrer_cpus(cpus);
	save_log(log,procesosid);
}

void mostrar_estados_rr(Log* log,Pcbs* pcbs_nuevos,Pcbs* pcbs_listos,Pcbs* pcbs_ejecutando,Pcbs* pcbs_bloqueados){
	inicio_log(log);
	log->FILE = fopen(log->PATH,"a");
	fprintf(log->FILE,"%s\n","COLA NUEVOS:");
	fprintf(log->FILE,"%s\n","============");
	fprintf(log->FILE,"%s\n",recorrer_cola(pcbs_nuevos));
	fprintf(log->FILE,"%s\n","COLA LISTOS:");
	fprintf(log->FILE,"%s\n","===========");
	fprintf(log->FILE,"%s\n",recorrer_cola(pcbs_listos));
	fprintf(log->FILE,"%s\n","COLA EJECUTANDO:");
	fprintf(log->FILE,"%s\n","============");
	fprintf(log->FILE,"%s\n",recorrer_cola(pcbs_ejecutando));
	fprintf(log->FILE,"%s\n","COLA BLOQUEADOS:");
	fprintf(log->FILE,"%s\n","===========");
	fprintf(log->FILE,"%s\n",recorrer_cola(pcbs_bloqueados));
	fclose(log->FILE);
}
