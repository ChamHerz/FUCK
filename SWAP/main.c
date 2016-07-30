#include <stdio.h>
#include "swap.h"

#define PATH_SWAP_CONF "swap.conf" //Archivo de configuracion
#define PATH_SWAP_LOG "swap.log" //Archivo de configuracion

main(int argc, char *argv[]){
	Swap* swap = new_swap();

	if(argc==2) {
		//significa que cargo un parametro
		if(string_equals_ignore_case(argv[1],"-v")){
			printf("Visor Activado.\n");
			swap->MONITOR_ACTIVADO = 1;
			swap->FILE_MONITOR = new_log(PATH_SWAP_LOG);
		}
	}

	swap->MONITOR_ACTIVADO = 1;
	swap->FILE_MONITOR = new_log(PATH_SWAP_LOG);

	set_path_archivo_configuracion(swap,PATH_SWAP_CONF);

	cargar_archivo_configuracion(swap);

	swap_iniciar(swap);

	return 0;
}
