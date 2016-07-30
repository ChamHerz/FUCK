#include "nucleo.h"

#define PATH_NUCLEO_CONF "nucleo.conf" //Archivo de configuracion
#define PATH_NUCLEO_MONITOR "rr.log" //Archivo de monitor

main(int argc, char *argv[]){

	Nucleo* nucleo = new_nucleo();

	if(argc==2) {
		//significa que cargo un parametro
	    if(string_equals_ignore_case(argv[1],"-v")){
	    	printf("Visor Activado.\n");
	    	nucleo->MONITOR_ACTIVADO = 1;
	    	nucleo->FILE_MONITOR = new_log(PATH_NUCLEO_MONITOR);
	    }
	}
	nucleo->MONITOR_ACTIVADO = 1;
	nucleo->FILE_MONITOR = new_log(PATH_NUCLEO_MONITOR);

	set_path_archivo_configuracion(nucleo,PATH_NUCLEO_CONF);

	cargar_archivo_configuracion(nucleo);

	crear_estructuras_administrativas(nucleo);

	nucleo_iniciar(nucleo);

	return 0;
}
