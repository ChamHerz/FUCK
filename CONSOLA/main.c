#include "consola.h"

#define PATH_CONSOLA_CONF "consola.conf" //Archivo de configuracion

main(int argc, char *argv[]){

	Consola* consola = new_consola();

	set_path_archivo_configuracion(consola,PATH_CONSOLA_CONF);

	cargar_archivo_configuracion(consola);

	set_path_ansisop(consola,argv[1]);

	consola_inciar(consola);

	return 0;
}
