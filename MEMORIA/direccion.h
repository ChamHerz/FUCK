#ifndef DIRECCION_H_
#define DIRECCION_H_

typedef struct direccion Direccion;

struct direccion {
	//PROPIEDADES
	int PID;
	int PAG;
	int OFFSET;
	int SIZE;
	int BUFFER;
};

#endif /* DIRECCION_H_ */
