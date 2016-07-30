#ifndef LOG_H_
#define LOG_H_

#include "pcbs.h"
#include "cpus.h"

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

void mostrar_estado_nuevos(Log* log, Pcbs* pcbs);

void mostrar_estado_finalizados(Log* log, Pcbs* pcbs);

#endif /* LOG_H_ */
