#include <commons/config.h>
#include <commons/string.h>
#include "swap.h"
#include "bitmap.h"
#include <pthread.h>
#include <unistd.h>		//para el usleep
#include <stdio.h>
#include <stdlib.h>
#include "programas.h"
#include "servidor.h"
#include "ansisop.h"
#include "direccion.h"

Swap* new_swap(){
	Swap * s;
	s = malloc(sizeof(struct swap));
	s->programas = new_programas();
	s->MONITOR_ACTIVADO = 0;
	pthread_mutex_init(&s->sem_memoria, NULL);
	//s->FILE_SWAP = new_log(PATH_LOG_SWAP);
	return s;
}

//Destructor
void destroy_swap(Swap* swap){
	destroy_programas(swap->programas);
	free(swap);
}

void set_path_archivo_configuracion(Swap* swap,char* PATH_SWAP_CONF){
	swap->path_conf_swap = PATH_SWAP_CONF;
}

void cargar_archivo_configuracion(Swap* swap){
	//Inicio del Nucleo - Cargar archivo
	printf("Cargando parametros... \n");
	t_config* config_file = config_create(swap->path_conf_swap);
	swap->PUERTO = config_get_string_value(config_file,"PUERTO");
	swap->NOMBRE_SWAP = config_get_string_value(config_file,"NOMBRE_SWAP");
	swap->CANT_PAGINAS = config_get_int_value(config_file,"CANT_PAGINAS");
	swap->SIZE_PAGINA = config_get_int_value(config_file,"SIZE_PAGINA");
	swap->RETARDO_ACCESSO = 1000 * config_get_int_value(config_file,"RETARDO_ACCESSO");
	swap->RETARDO_COMPACTACION = 1000 * config_get_int_value(config_file,"RETARDO_COMPACTACION");
	printf("Parametros cargados desde el archivo configuracion \n");

	//swap->programas->pag_totales = swap->CANT_PAGINAS;

	//hilo_mostrar_estado_swap(swap);
}

void swap_iniciar(Swap* swap){
	swap_crear_archivo(swap);

	swap_crear_bitmap(swap);

	//swap->hilo_para_memoria = crear_hilo_un_parametro(hilo_para_memoria,swap);

	//Esperar a que terminen los hilos creados
	//pthread_join(swap->hilo_para_memoria, NULL);
	swap->DESCRIPTOR = abrir_socket(swap->PUERTO);

	if(swap->DESCRIPTOR == -1){
		printf("\nError al abrir puerto con Memoria, reconecctando...\n");
	}
	else{
		printf("Servidor SWAP abierto en, descriptor: %d en puerto: %s \n", swap->DESCRIPTOR,swap->PUERTO);
	}

	swap->DESCRIPTOR_MEMORIA = aceptar_conexion_cliente(swap->DESCRIPTOR);

	printf("Conexion establecida con Memoria: descriptor: %d en puerto: %s \n", swap->DESCRIPTOR,swap->PUERTO);

	int dato_leido;
	int resultado_lectura;
	int bucle = 1;
	while (bucle == 1){
		pthread_mutex_lock(&swap->sem_memoria); //Solo hago una tarea a la vez con memoria
		resultado_lectura = leer_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)&dato_leido,sizeof(int));
		if(resultado_lectura == 0){
			printf("\nConexion con memoria perdida\n");
			bucle = 0;
		}
		else{
			bucle = realizar_accion_con_memoria(swap,dato_leido);
		}
		pthread_mutex_unlock(&swap->sem_memoria);
	}

	printf("\nFin de Swap\n");
}

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Swap*),Swap* swap){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (Swap*)swap);

	return thd1;
}

void hilo_para_memoria(Swap* swap){


}

void hilo_monitor(Swap* swap){
	mostrar_estado_swap(swap->FILE_MONITOR,swap->programas,swap->BITMAP);
//	pthread_exit(NULL);
}

void crear_hilo_un_para_un_monitor(void(*funcion_externa)(Swap*),Swap* swap){
	pthread_attr_t atributos;
	pthread_attr_init(&atributos);
	pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_DETACHED); //Liberar recursos automaticamente
	pthread_t thd1;
	pthread_create (&thd1, &atributos, (void*)funcion_externa, (Swap*)swap);
}

int realizar_accion_con_memoria(Swap* swap,int dato_leido){
	int dato_enviar;
	if (dato_leido == 25){
		printf("\n25 - Solicitud de Size AnSISOP desde memoria\n");
		recibir_size_ansisop(swap);
		return 1;
	}
	if (dato_leido == 31){
		printf("\n31 - Solicitud de Direccion desde memoria\n");
		dato_enviar = 32; //Informo a SWAP esta listo para la direccion
		escribir_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
		recibir_direccion_memoria(swap);
		return 1;
	}
	if (dato_leido == 63){
		printf("\n63 - Solicitud de Direccion desde memoria para escribir\n");
		dato_enviar = 64; //Informo a SWAP esta listo para la direccion para escribir
		escribir_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
		recibir_direccion_memoria_almacenar(swap);
		return 1;
	}
	if (dato_leido == 57){
		printf("\n57 - Solicitud de Borrado de programa\n");
		int pid;
		leer_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)&pid,sizeof(int));
		printf("\nPID: %d\n",pid);
		borrar_programa_ansisop(swap,pid);
		return 1;
	}
	return 1;
}

void borrar_programa_ansisop(Swap* swap,int pid){
	Programa* programa = buscar_programa(swap->programas,pid);
	if (programa != NULL){//puede que se haya borrado anteriormente
		clear_pool_bit(swap->BITMAP,programa->START_PAG,programa->START_PAG + programa->CANT_PAG);
		//borrar_ansisop_del_swap(swap->NOMBRE_SWAP,programa);
		borrar_programa(swap->programas,pid);
		printf("Programa Borrado");
		if(swap->MONITOR_ACTIVADO)
			hilo_monitor(swap);
	}
}

void recibir_size_ansisop(Swap* swap){
	Ansisop* ansisop = new_ansisop();
	leer_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)ansisop,sizeof *ansisop);
	ansisop->CADENA = malloc(ansisop->SIZE);
	leer_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)ansisop->CADENA,ansisop->SIZE);
	int pag_necesarias = calcular_cant_pag_necesarias(swap,ansisop);
	printf("\nPaginas necesarias para almacenar el ansisop: %d\n",pag_necesarias);

	int responder;
	int entra_contiguo;
	int entra_ansisop = controlar_solo_si_entra(swap->BITMAP,pag_necesarias);
	if (entra_ansisop){
		entra_contiguo = controlar_si_entra_contiguo(swap->BITMAP,pag_necesarias);
		if (entra_contiguo){
			Programa* programa = new_programa();
			programa->PID = ansisop->PID;
			programa->SIZE = ansisop->SIZE;
			programa->PAG_STACK = ansisop->PAG_STACK;
			programa->SIZE_PAGE = swap->SIZE_PAGINA;
			set_bitmap(swap->BITMAP,ansisop,programa,pag_necesarias);
			escribir_swap(swap->NOMBRE_SWAP,ansisop,programa);
			agregar_programa(swap->programas,programa);
			responder = 71;//Avisa que entra normalmente
		}
		else{
			//compactar
			compactar_swap(swap);

			//almacenar el progama
			Programa* programa = new_programa();
			programa->PID = ansisop->PID;
			programa->SIZE = ansisop->SIZE;
			programa->SIZE_PAGE = swap->SIZE_PAGINA;
			set_bitmap(swap->BITMAP,ansisop,programa,pag_necesarias);
			escribir_swap(swap->NOMBRE_SWAP,ansisop,programa);
			agregar_programa(swap->programas,programa);

			responder = 71;//Avisa que se va compactara swap
		}
		if(swap->MONITOR_ACTIVADO)
			hilo_monitor(swap);
	}
	else{
		responder = 72;//no entra
	}

	escribir_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)&responder,sizeof(int));
}

void compactar_swap(Swap* swap){
	//Necesito compactar
	printf("\nEntro a Compactacion\n");

	char** VECTOR_COMPACTACION = calloc(swap->CANT_PAGINAS,swap->SIZE_PAGINA);

	int index;
	Direccion* direccion = new_direccion_vacia();
	direccion->OFFSET = 0;
	direccion->PAG = 0;
	direccion->SIZE = swap->SIZE_PAGINA;
	for (index = 0; index <= swap->CANT_PAGINAS; index++){
		VECTOR_COMPACTACION[index] = leer_archivo(swap->NOMBRE_SWAP,index,direccion,swap->SIZE_PAGINA);
	}

	//No Borro el swap para que el vector.ansisop muestro los pedazos de codigo
	//swap_crear_archivo(swap);

	//Pongo a cero a todos los bitmap
	clear_pool_bit(swap->BITMAP,0,swap->CANT_PAGINAS);

	int cant_programas = list_size(swap->programas->lista);
	int index_prog_actual;
	Programa* programa_actual;
	int index_pag_actual;
	int hasta_pag;
	int index_swap = 0;
	int START_PAG;
	int bit_end;
	printf("\nBitMap Borrado\n");
	for (index_prog_actual = 0; index_prog_actual < cant_programas; index_prog_actual++){
		programa_actual = list_get(swap->programas->lista,index_prog_actual);
		hasta_pag = programa_actual->START_PAG + programa_actual->CANT_PAG;
		START_PAG = index_swap;
		direccion->PID = programa_actual->PID;
		for(index_pag_actual = programa_actual->START_PAG;index_pag_actual < hasta_pag;index_pag_actual++){
			char* pagina_a_escribir = VECTOR_COMPACTACION[index_pag_actual];
			escribir_archivo(swap->NOMBRE_SWAP,index_swap,direccion,swap->SIZE_PAGINA,pagina_a_escribir);
			index_swap++;
		}
		programa_actual->START_PAG = START_PAG;

		bit_end = programa_actual->START_PAG + programa_actual->CANT_PAG;
		set_pool_bit(swap->BITMAP,programa_actual->START_PAG,bit_end);
	}

	printf("\nCompactado Exitosamente\n");

	usleep(swap->RETARDO_COMPACTACION);
}

int calcular_cant_pag_necesarias(Swap* swap, Ansisop* ansisop){
	int cantidad_necesaria = ansisop->SIZE / swap->SIZE_PAGINA;

	if (ansisop->SIZE % swap->SIZE_PAGINA != 0){
		cantidad_necesaria++;
	}

	return cantidad_necesaria + ansisop->PAG_STACK;
}

void swap_crear_archivo(Swap* swap){
	char* name = swap->NOMBRE_SWAP;
	int size = swap->SIZE_PAGINA;
	int cant = swap->CANT_PAGINAS;
	char* comando = string_from_format("dd if=/dev/zero of=%s bs=%d count=%d",name,size,cant);
	printf("\n%s\n",comando);
	system(comando);
	printf("Archivo de swap creado exitosamente\n");
}

void swap_crear_bitmap(Swap* swap){
	//Se crea un bitmap del tamano necesario para las paginas configuradas
	Bitmap* bitmap = new_bitmap(swap->CANT_PAGINAS);
	swap->BITMAP = bitmap;
	printf("Se creo bitmap de %d palabras\n",swap->BITMAP->size_mapa);
}

void recibir_direccion_memoria(Swap* swap){
	printf("aqui recibo la direccion de memoria\n");
	Direccion* direccion = new_direccion_vacia();
	leer_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)direccion,sizeof *direccion);
	printf("PID: %d\n",direccion->PID);
	printf("PAG: %d\n",direccion->PAG);
	printf("OFFSET %d\n",direccion->OFFSET);
	printf("SIZE %d\n",direccion->SIZE);

	//Buscar pag donde comienza el programa por PID
	char* cadena_instruccion;
	Programa* programa = buscar_programa(swap->programas,direccion->PID);
	if (programa != NULL){
		printf("Pagina buscada desde lista %d\n",programa->START_PAG);

		//Extraer direccion
		cadena_instruccion = leer_archivo(swap->NOMBRE_SWAP,programa->START_PAG,direccion,swap->SIZE_PAGINA);
		printf("\n%s\n",cadena_instruccion);

		usleep(swap->RETARDO_ACCESSO);
		escribir_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)cadena_instruccion,swap->SIZE_PAGINA);
	}
	else{
		//el programa pudo haber sido borrado.... y luego cae la solicitud de leer una pagina.
		//en este caso le mando una pagina vacia para que no rompa.
		//cuando memoria intente enviar al cpu... le enviara a un descriptor -1;
		cadena_instruccion = calloc(swap->SIZE_PAGINA,sizeof(char));
		escribir_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)cadena_instruccion,swap->SIZE_PAGINA);
	}
}

void recibir_direccion_memoria_almacenar(Swap* swap){
	printf("aqui recibo la direccion de memoria que se modificara\n");
	Direccion* direccion = new_direccion_vacia();
	leer_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)direccion,sizeof *direccion);
	printf("PID: %d\n",direccion->PID);
	printf("PAG: %d\n",direccion->PAG);
	printf("OFFSET %d\n",direccion->OFFSET);
	printf("SIZE %d\n",direccion->SIZE);

	//creo la pagina
	char* pagina = malloc(swap->SIZE_PAGINA);
	leer_descriptor(swap->DESCRIPTOR_MEMORIA,(char*)pagina,swap->SIZE_PAGINA);

	//Buscar pag donde comienza el stack del programa por PID
	Programa* programa = buscar_programa(swap->programas,direccion->PID);
	printf("Pagina donde arranca el programa%d\n",programa->START_PAG);

	//Guardar direccion
	escribir_archivo(swap->NOMBRE_SWAP,programa->START_PAG,direccion,swap->SIZE_PAGINA,pagina);

	usleep(swap->RETARDO_ACCESSO);
}
