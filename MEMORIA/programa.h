#ifndef PROGRAMA_H_
#define PROGRAMA_H_

typedef struct programa Programa;

struct programa {
	//PROPIEDADES
	int PID;
	int CANT_MARCOS;
	int PUNTERO_CLOCK;
};

//Constructor
Programa* new_programa(int PID);

//Destructor
void destroy_programa(Programa* programa);

#endif /* PROGRAMA_H_ */
