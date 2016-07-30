#ifndef MARCO_H_
#define MARCO_H_

typedef struct marco Marco;

struct marco {
	//PROPIEDADES
	int MARCOID;
	int PID;
	int PAGINAID;
	int USADA;
	int MODIFICADA;
};

//Constructor
Marco* new_marco(int MARCOID,int PID,int PAGID,int MODIFICADA);

//Destructor
void destroy_marco(Marco* marco);

#endif /* MARCO_H_ */
