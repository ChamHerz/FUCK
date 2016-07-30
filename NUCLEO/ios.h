#ifndef IOS_H_
#define IOS_H_

#include "io.h"
#include <commons/collections/list.h>

typedef struct ios Ios;

struct ios {
	//PROPIEDADES
	int PID_actual;
	char* io_id_actual;
	int io_cantidad_actual;
	t_list* lista;
};

Ios* new_ios();

void destroy_ios(Ios* ios);

void agregar_io(Ios* ios, Io* io);

Io* buscar_io(Ios* ios, char* io_id);

#endif /* IOS_H_ */
