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

Direccion* new_direccion(int byte_start, int byte_cant, int pag_size);

Direccion* new_direccion_vacia();

#endif /* DIRECCION_H_ */
