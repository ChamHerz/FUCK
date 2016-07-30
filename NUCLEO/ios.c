#include "ios.h"
#include <stdlib.h>
//constructor
Ios* new_ios(){
	Ios* i;
	i = malloc(sizeof(struct ios));
	i->lista = list_create();
	return i;
}

//Destructor
void destroy_ios(Ios* ios){
	free(ios);
}

void agregar_io(Ios* ios, Io* io){
	list_add(ios->lista,io);
}

Io* buscar_io(Ios* ios, char* io_id){
	bool condicion_busqueda(Io* io){
		return string_equals_ignore_case(io->io_id,io_id);
	}

	return list_find(ios->lista,(void*)condicion_busqueda);
}
