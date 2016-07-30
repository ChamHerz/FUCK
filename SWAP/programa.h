#ifndef PROGRAMA_H_
#define PROGRAMA_H_

typedef struct programa Programa;

struct programa {
	//PROPIEDADES
	int PID;
	int SIZE;
	int CANT_PAG;
	int START_PAG;
	//int START_STACK;
	int PAG_STACK;
	int SIZE_PAGE;
};

void calcular_cant_pag(Programa* programa,int size_pag);

Programa* new_programa();

void destroy_programa(Programa* programa);

#endif /* PROGRAMA_H_ */
