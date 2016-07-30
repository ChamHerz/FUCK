#include "io.h"
#include <stdlib.h>

//constructor
Io* new_io(char* io_id,int io_sleep){
	Io* i;
	i = malloc(sizeof(struct io));
	i->io_id = io_id;
	i->io_sleep = io_sleep;

	//MUTEX
	pthread_mutex_init(&i->MUTEX, NULL);
	return i;
}

//Destructor
void destroy_io(Io* io){
	free(io);
}
