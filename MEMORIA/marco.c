#include "marco.h"
#include <stdlib.h>

//constructor
Marco* new_marco(int MARCOID,int PID,int PAGID,int MODIFICADA){
	Marco* m;
	m = malloc(sizeof(struct marco));
	m->PID = PID;
	m->MARCOID = MARCOID;
	m->PAGINAID = PAGID;
	m->USADA = 0;
	m->MODIFICADA = MODIFICADA;
	return m;
}

//Destructor
void destroy_marco(Marco* marco){
	free(marco);
}
