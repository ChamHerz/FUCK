#ifndef PCB_H_
#define PCB_H_

#include <parser/parser.h>
#include <parser/metadata_program.h>

typedef struct address Address;

struct address {
	int POS; //4
	char ID; //8
	int PAG;  //12
	int OFFSET; //16
	int SIZE;  //20
};

typedef struct stack Stack;

struct stack{
	Address* RET_VAR;  //4
	int* RET_POS;  //8
	int POS;  //12
	Address* VARS;  //16
	int VARS_SIZE;  //20
};

typedef struct pcb Pcb;

struct pcb {
	//PROPIEDADES
	int PID; //4
	int PC;  //8
	int INSTRUCCION_START;//12
	t_intructions* INSTRUCCIONES; //16
	int INSTRUCCIONES_SIZE; //20
	int INSTRUCCIONES_CANT; //24
	char* ETIQUETAS; //28
	int ETIQUETAS_SIZE;  //32
	int ETIQUETAS_CANT; //36
	Stack* STACK;  //40
	int QUANTUM; //44
	int QUANTUM_SLEEP; //48
	int QUANTUM_ACTUAL; //52
	int CANT_PAG_CODIGO; //56
	int PAG_STACK; //60
	int ESTADO_FINAL;//64
	int FLAG_SISGUR1; //68
	int PAG_SIZE; //72
	char SEMAFORO; //76
};

void destroy_pcb(Pcb* pcb);

#endif /* PCB_H_ */
