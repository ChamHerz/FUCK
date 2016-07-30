#include "memoria.h"

#define PATH_MEMORIA_CONF "memoria.conf" //Archivo de configuracion
#define PATH_MEMORIA_MONITOR "mem.log" //Archivo de monitor

main(int argc, char *argv[]){

	Memoria* memoria = new_memoria();

	if(argc==2) {
		//significa que cargo un parametro
		if(string_equals_ignore_case(argv[1],"-v")){
			printf("Visor Activado.\n");
			memoria->MONITOR_ACTIVADO = 1;
			memoria->FILE_MONITOR = new_log(PATH_MEMORIA_MONITOR);
		}
	}

	memoria->MONITOR_ACTIVADO = 1;
	memoria->FILE_MONITOR = new_log(PATH_MEMORIA_MONITOR);

	set_path_archivo_configuracion(memoria,PATH_MEMORIA_CONF);

	cargar_archivo_configuracion(memoria);

	crear_estructuras_administrativas(memoria);

	memoria_inciar(memoria);

	return 0;
}
