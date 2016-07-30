#ifndef IO_H_
#define IO_H_

#include <pthread.h>

typedef struct io Io;

struct io {
	//PROPIEDADES
	char* io_id;
	int io_sleep;

	//MUTEX
	pthread_mutex_t MUTEX;
};
//Constructor
Io* new_io(char* io_id,int io_sleep);

//Destructor
void destroy_io(Io* io);

#endif /* IO_H_ */
