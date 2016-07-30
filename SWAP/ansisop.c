#include "ansisop.h"
#include <stdlib.h>

//constructor
Ansisop* new_ansisop(){
	Ansisop* a;
	a = malloc(sizeof(struct ansisop));
	return a;
}

//Destructor
void destroy_ansisop(Ansisop* ansisop){
	free(ansisop);
}
