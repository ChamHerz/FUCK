#ifndef TLB_H_
#define TLB_H_

typedef struct tlb Tlb;

struct tlb {
	//PROPIEDADES
	int TLBID;
	int PID;
	int NRO_PAGINA;
	int NRO_MARCO;
	int PRESENTE;
	int REFERENCIADA;
};

//Constructor
Tlb* new_tlb(int PID,int NRO_PAGINA,int MARCO_ID);

//Destructor
void destroy_tlb(Tlb* tlb);

#endif /* TLB_H_ */
