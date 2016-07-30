#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		//para el usleep
#include "nucleo.h"
#include "servidor.h"
#include "consola.h"
#include "consolas.h"
#include "file.h"
#include "ansisop.h"
#include "pcbs.h"
#include <commons/config.h>
#include <commons/string.h>

//constructor
Nucleo* new_nucleo(){
	Nucleo * n;
	n = malloc(sizeof(struct nucleo));
	n->PID_ACTUAL = 0;
	n->MONITOR_ACTIVADO = 0;
	n->consolas = new_consolas();
	n->cpus = new_cpus();
	n->variables = new_variables();
	n->semaforos = new_semaforos();
	n->ios = new_ios();

	//COLAS
	n->cola_nuevos = new_pcbs();
	n->cola_listos = new_pcbs();
	n->cola_ejecutando = new_pcbs();
	n->cola_bloqueados = new_pcbs();
	n->cola_finalizados = new_pcbs();
	n->lista_bloqueados = new_pidblocks();

	//SEMAFOROS
	pthread_mutex_init(&n->sem_memoria, NULL);
	pthread_mutex_init(&n->sem_pcb, NULL);
	pthread_mutex_init(&n->sem_cpu, NULL);
	pthread_mutex_init(&n->sem_semaforos, NULL);
	pthread_mutex_init(&n->sem_variables_glo, NULL);
	return n;
}

NucleoConsola* new_nucleoconsola(Nucleo* nucleo,Consola* consola){
	NucleoConsola * nc;
	nc = malloc(sizeof(struct nucleoconsola));
	nc->nucleo = nucleo;
	nc->consola = consola;
	return nc;
}

NucleoCpu* new_nucleocpu(Nucleo* nucleo,Cpu* cpu){
	NucleoCpu * nc;
	nc = malloc(sizeof(struct nucleocpu));
	nc->nucleo = nucleo;
	nc->cpu = cpu;
	return nc;
}

NucleoSemaforo* new_nucleosemaforo(Nucleo* nucleo, Semaforo* semaforo){
	NucleoSemaforo * ns;
	ns = malloc(sizeof(struct nucleosemaforo));
	ns->nucleo = nucleo;
	ns->semaforo = semaforo;
	return ns;
}

NucleoIo* new_nucleoio(Nucleo* nucleo, Io* io, Pidblock* pidblock){
	NucleoIo * ni;
	ni = malloc(sizeof(struct nucleoio));
	ni->nucleo = nucleo;
	ni->io = io;
	ni->pidblock = pidblock;
	return ni;
}

void destroy_nucleoconsola(NucleoConsola* nucleoconsola){
	free(nucleoconsola);
}

void destroy_nucleocpu(NucleoCpu* nucleocpu){
	free(nucleocpu);
}

void destroy_nucleosemaforo(NucleoSemaforo* nucleosemaforo){
	free(nucleosemaforo);
}

void destroy_nucleoio(NucleoIo* nucleoio){
	free(nucleoio);
}

void set_path_archivo_configuracion(Nucleo* nucleo,char* path){
	nucleo->path_conf_nucleo = path;
}

void cargar_archivo_configuracion(Nucleo* nucleo){
	//Inicio del Nucleo - Cargar archivo
	printf("\niniciando carga de archivo\n");
	printf("\nfile config: %s\n",nucleo->path_conf_nucleo);

	nucleo->FILE_CONFIGURACION = config_create(nucleo->path_conf_nucleo);
	nucleo->PUERTO_PROG = config_get_int_value(nucleo->FILE_CONFIGURACION,"PUERTO_PROG");
	//int size = string_length(config_get_string_value(nucleo->FILE_CONFIGURACION,"IP_MEMORIA"));
	//nucleo->IP_MEMORIA = calloc(size,sizeof(char));
	//memcpy(nucleo->IP_MEMORIA,config_get_string_value(nucleo->FILE_CONFIGURACION,"IP_MEMORIA"),size);
	nucleo->IP_MEMORIA = config_get_string_value(nucleo->FILE_CONFIGURACION,"IP_MEMORIA");
	nucleo->PUERTO_MEMORIA = config_get_string_value(nucleo->FILE_CONFIGURACION,"PUERTO_MEMORIA");
	nucleo->PUERTO_CPU = config_get_int_value(nucleo->FILE_CONFIGURACION,"PUERTO_CPU");
	nucleo->SIZE_PAGINA = config_get_int_value(nucleo->FILE_CONFIGURACION,"SIZE_PAGINA");
	nucleo->PAG_STACK = config_get_int_value(nucleo->FILE_CONFIGURACION,"PAG_STACK");
	nucleo->QUANTUM = config_get_int_value(nucleo->FILE_CONFIGURACION,"QUANTUM");
	nucleo->QUANTUM_SLEEP = 1000 * config_get_int_value(nucleo->FILE_CONFIGURACION,"QUANTUM_SLEEP");
	nucleo->SEM_ID = config_get_array_value(nucleo->FILE_CONFIGURACION,"SEM_ID");
	nucleo->SEM_INIT = config_get_array_value(nucleo->FILE_CONFIGURACION,"SEM_INIT");
	nucleo->IO_ID = config_get_array_value(nucleo->FILE_CONFIGURACION,"IO_ID");
	nucleo->IO_SLEEP = config_get_array_value(nucleo->FILE_CONFIGURACION,"IO_SLEEP");
	nucleo->SHARED_VARS = config_get_array_value(nucleo->FILE_CONFIGURACION,"SHARED_VARS");
	printf("\ncarga de archivo Finalizada\n");

	//config_destroy(nucleo->FILE_CONFIGURACION);
}

void crear_estructuras_administrativas(Nucleo* nucleo){
	crear_lista_de_semaforos(nucleo);
	crear_lista_de_io(nucleo);
	crear_lista_de_variables_globales(nucleo);
}

void crear_lista_de_variables_globales(Nucleo* nucleo){
	char** vector_id = nucleo->SHARED_VARS;
	int vector_size_id = 0;
	while(vector_id[vector_size_id] != NULL){
		Variable* variable = new_variable(vector_id[vector_size_id],0);//Se inicializan en 0
		agregar_variable(nucleo->variables,variable);
		vector_size_id = vector_size_id + 1;
		printf("\nVariable Global creada id: %s valor: %d\n",variable->id_variable,variable->valor_variable);
	}
}

void crear_lista_de_semaforos(Nucleo* nucleo){
	char** vector_id = nucleo->SEM_ID;
	char** vector_valores = nucleo->SEM_INIT;
	int vector_size_id = 0;
	int valor;
	while(vector_id[vector_size_id] != NULL){
		valor = convertir_cadena_a_entero(vector_valores[vector_size_id]);
		Semaforo* semaforo = new_semaforo(vector_id[vector_size_id][0],valor);
		agregar_semaforo(nucleo->semaforos,semaforo);
		vector_size_id += 1;
		printf("\nSemaforo creado id: %c valor: %d\n",semaforo->id_semaforo,semaforo->valor_semaforo);
	}
}

void crear_lista_de_io(Nucleo* nucleo){
	char** vector_id = nucleo->IO_ID;
	char** vector_valores = nucleo->IO_SLEEP;
	int vector_size_id = 0;
	int valor;
	while (vector_id[vector_size_id] != NULL){
		valor = convertir_cadena_a_entero(vector_valores[vector_size_id]);
		Io* io = new_io(vector_id[vector_size_id],valor);
		agregar_io(nucleo->ios,io);
		vector_size_id += 1;
		printf("\nIO creado id: %s valor: %d\n",io->io_id,io->io_sleep);
	}
}

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Nucleo*),Nucleo* nucleo){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (Nucleo*)nucleo);

	return thd1;
}

pthread_t crear_hilo_un_para_una_consola(void(*funcion_externa)(NucleoConsola*),NucleoConsola* nucleoconsola){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (NucleoConsola*)nucleoconsola);

	return thd1;
}

pthread_t crear_hilo_un_para_un_cpu(void(*funcion_externa)(NucleoCpu*),NucleoCpu* nucleocpu){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (NucleoCpu*)nucleocpu);

	return thd1;
}

void crear_hilo_un_para_un_monitor(void(*funcion_externa)(Nucleo*),Nucleo* nucleo){
	pthread_attr_t atributos;
	pthread_attr_init(&atributos);
	pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_DETACHED); //Liberar recursos automaticamente
	pthread_t thd1;
	pthread_create (&thd1, &atributos, (void*)funcion_externa, (Nucleo*)nucleo);
}

void crear_hilo_un_para_un_semaforo(void(*funcion_externa)(NucleoSemaforo*),NucleoSemaforo* nucleosemaforo){
	pthread_attr_t atributos;
	pthread_attr_init(&atributos);
	pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_DETACHED); //Liberar recursos automaticamente
	pthread_t thd1;
	pthread_create (&thd1, &atributos, (void*)funcion_externa, (NucleoSemaforo*)nucleosemaforo);
}

void crear_hilo_un_para_un_io(void(*funcion_externa)(NucleoIo*),NucleoIo* nucleoio){
	pthread_attr_t atributos;
	pthread_attr_init(&atributos);
	pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_DETACHED); //Liberar recursos automaticamente
	pthread_t thd1;
	pthread_create (&thd1, &atributos, (void*)funcion_externa, (NucleoIo*)nucleoio);
}

void hilo_para_una_consola(NucleoConsola* nucleoconsola){
	Nucleo* nucleo = nucleoconsola->nucleo;
	Consola* consola = nucleoconsola->consola;
	destroy_nucleoconsola(nucleoconsola);

	printf("\nHilo para una consola %d\n",consola->descriptor);
	int leer_dato;
	int resultado;
	int cerro_conexion = 0;
	while(cerro_conexion == 0){
		resultado = leer_descriptor(consola->descriptor,(char*)&leer_dato,sizeof(int));
		if(resultado <= 0){
			consola->descriptor = -1;
			printf("consola cerrada");
			int pcb_borrado = 0;
			Pcb* pcb;
			Pcb* pcb_sacar;
			while (pcb_borrado == 0){
				pthread_mutex_lock(&nucleo->sem_pcb);
				pcb = buscar_pcb(nucleo->cola_bloqueados,consola->PID);
				if (pcb != NULL){
					pcb_sacar = sacar_pcb(nucleo->cola_bloqueados,consola->PID);
				}
				pcb = buscar_pcb(nucleo->cola_listos,consola->PID);
				if (pcb != NULL){
					pcb_sacar = sacar_pcb(nucleo->cola_listos,consola->PID);
				}
				if (pcb_sacar != NULL){
					agregar_pcb(nucleo->cola_finalizados,pcb_sacar);
					avisar_a_memoria_fin_programa(nucleo,consola->PID);
					pcb_borrado = 1;
				}
				pthread_mutex_unlock(&nucleo->sem_pcb);
				usleep(1000000);
			}
			close(consola->descriptor);
			borrar_consola(nucleo->consolas,consola->id_consola);
			cerro_conexion = 1;
		}
		else{
			cerro_conexion = realizar_accion_consola(nucleo,consola,leer_dato);
		}
	}
}

int realizar_accion_consola(Nucleo* nucleo, Consola* consola, int accion){
	printf("\nAccion: %d\n",accion);
	int dato_enviar;
	int respuesta_de_memoria;
	if (accion == 21){
		//consola eviara el size y el ansisop
		Ansisop* ansisop = new_ansisop();
		//Listo
		dato_enviar = 22;
		escribir_descriptor(consola->descriptor,(char*)&dato_enviar,sizeof(int));
		leer_descriptor(consola->descriptor,(char*)&ansisop->SIZE,sizeof(int));
		ansisop->CADENA = malloc(ansisop->SIZE);
		leer_descriptor(consola->descriptor,(char*)ansisop->CADENA,ansisop->SIZE);

		printf("\n%s\n",ansisop->CADENA);

		//Evita que dos consolas tomen el descriptor de memoria a la vez
		//y la estructura de Ansisop
		pthread_mutex_lock(&nucleo->sem_memoria);
		nucleo->PID_ACTUAL++;
		ansisop->PID = nucleo->PID_ACTUAL;
		ansisop->PAG_STACK = nucleo->PAG_STACK;
		printf("Size de ANSISOP recibido: %d\n",ansisop->SIZE);

		int resultado = verificar_swap_espacio(nucleo,ansisop);
		if (resultado == 71){ //Entra en SWAP
			Pcb* pcb = new_pcb();
			crear_pcb(nucleo,ansisop,pcb);
			consola->PID = pcb->PID; //Cargo el Pid en la consola
			pthread_mutex_lock(&nucleo->sem_pcb);
			agregar_pcb(nucleo->cola_nuevos,pcb);
			pthread_mutex_unlock(&nucleo->sem_pcb);
			printf("PCB agregado a la cola de nuevos, PID: %d\n",pcb->PID);
		}
		if (resultado == 72){
			//No entro
		}
		pthread_mutex_unlock(&nucleo->sem_memoria);
		escribir_descriptor(consola->descriptor,(char*)&resultado,sizeof(int));
		return 0;
	}
	return 0;
}

void crear_pcb(Nucleo* nucleo,Ansisop* ansisop,Pcb* pcb){
	pcb->PID = ansisop->PID;
	pcb->CANT_PAG_CODIGO = calcular_cant_pag_necesarias(nucleo,ansisop->SIZE) - nucleo->PAG_STACK;
	pcb->PAG_STACK = nucleo->PAG_STACK;
	pcb->PAG_SIZE = nucleo->SIZE_PAGINA;
	pcb->QUANTUM = nucleo->QUANTUM;
	pcb->QUANTUM_SLEEP = nucleo->QUANTUM_SLEEP;
	pcb->QUANTUM_ACTUAL = 1; //arranca en el quantum 1
	t_metadata_program* programa;
	programa = metadata_desde_literal(ansisop->CADENA);
	pcb->INSTRUCCION_START = programa->instruccion_inicio;
	pcb->INSTRUCCIONES = programa->instrucciones_serializado;
	pcb->INSTRUCCIONES_SIZE = programa->instrucciones_size;

	//Control de instrucciones
	int index;
	for(index=0;index<pcb->INSTRUCCIONES_SIZE;index++){
		printf("\nbyte: %d size: %d\n",pcb->INSTRUCCIONES[index].start,pcb->INSTRUCCIONES[index].offset);
	}

	pcb->INSTRUCCIONES_CANT = programa->cantidad_de_funciones;
	pcb->ETIQUETAS_SIZE = programa->etiquetas_size;
	pcb->ETIQUETAS_CANT = programa->cantidad_de_etiquetas;
	pcb->ETIQUETAS = malloc(pcb->ETIQUETAS_SIZE);
	pcb->ETIQUETAS = programa->etiquetas;
	//Test de etiqueta
	t_puntero_instruccion retorno = metadata_buscar_etiqueta("etiqueta",pcb->ETIQUETAS,pcb->ETIQUETAS_SIZE);
	printf("La instruccion donde debe ir es %d\n",retorno);

	pcb->STACK = malloc(sizeof *pcb->STACK);
	pcb->STACK->POS = 0;
	pcb->STACK->VARS_SIZE = 0;
	pcb->STACK->VARS = malloc(pcb->STACK->VARS_SIZE * sizeof *pcb->STACK->VARS);
	pcb->STACK->RET_VAR = malloc((pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_VAR);
	pcb->STACK->RET_POS = malloc((pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_POS);
	printf("Se creo el PCB\n");

	printf("intrucciones_size %d\n",pcb->INSTRUCCIONES_SIZE);
	printf("intrucciones_cant %d\n",pcb->INSTRUCCIONES_CANT);
	printf("intrucciones_start %d\n",pcb->INSTRUCCION_START);
	printf("cant_pag_codigo %d\n",pcb->CANT_PAG_CODIGO);
}

int verificar_swap_espacio(Nucleo* nucleo,Ansisop* ansisop){
	int dato_enviar = 23;//Avisa que enviop ansisop_size y ansisop
	//int cant_pag_necesarias = calcular_cant_pag_necesarias(nucleo,ansisop_size);
	escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)ansisop,sizeof *ansisop);
	escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)ansisop->CADENA,ansisop->SIZE);
	printf("\n23 - Se envio size_ansisop %d \n",ansisop->SIZE);

	int resultado;
	leer_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&resultado,sizeof(int));
	return resultado;
}

int calcular_cant_pag_necesarias(Nucleo* nucleo, int ansisop_size){
	int cantidad_necesaria = ansisop_size / nucleo->SIZE_PAGINA;

	if (ansisop_size % nucleo->SIZE_PAGINA != 0){
		cantidad_necesaria++;
	}

	return cantidad_necesaria + nucleo->PAG_STACK;
}

void nucleo_iniciar(Nucleo* nucleo){
	nucleo->hilo_para_consolas = crear_hilo_un_parametro(hilo_para_consolas,nucleo);
	nucleo->hilo_para_memoria = crear_hilo_un_parametro(hilo_para_memoria,nucleo);
	nucleo->hilo_para_cpus = crear_hilo_un_parametro(hilo_para_cpus,nucleo);
	nucleo->hilo_para_round_robin = crear_hilo_un_parametro(hilo_para_round_robin,nucleo);
	nucleo->hilo_para_cambio_configuracion = crear_hilo_un_parametro(hilo_para_cambio_conf,nucleo);

	//Esperar a que terminen los hilos creados
	pthread_join(nucleo->hilo_para_consolas, NULL);
	pthread_join(nucleo->hilo_para_memoria, NULL);
	pthread_join(nucleo->hilo_para_cpus, NULL);
	pthread_join(nucleo->hilo_para_round_robin, NULL);
	pthread_join(nucleo->hilo_para_cambio_configuracion, NULL);
	printf("\nFin de Nucleo\n");
}

void hilo_para_cambio_conf(Nucleo* nucleo){
	int resultado_escuchar;
	while(1){
		//iniciar escucha es bloqueante
		resultado_escuchar = iniciar_escucha_archivo(nucleo->path_conf_nucleo);
		if(resultado_escuchar == 0){
			//se recarga los parametros porque se modifico
			pthread_mutex_lock(&nucleo->sem_pcb);
			//Por si esta creando un pcb.
			nucleo_recargar_quantums(nucleo);
			//cargar_archivo_configuracion(nucleo);
			pthread_mutex_unlock(&nucleo->sem_pcb);
		}
	}
}

void nucleo_recargar_quantums(Nucleo* nucleo){
	printf("\nSE MODIFICO EL ARCHIVO\n");
	usleep(1000000);
	t_config* config_file = config_create(nucleo->path_conf_nucleo);
	nucleo->QUANTUM = config_get_int_value(config_file,"QUANTUM");
	nucleo->QUANTUM_SLEEP = 1000 * config_get_int_value(config_file,"QUANTUM_SLEEP");
}

void hilo_para_round_robin(Nucleo* nucleo){
	while(1){
		usleep(1000000); //recorrer listas cada 1 segundo
		pthread_mutex_lock(&nucleo->sem_pcb);
		if(esta_vacia_la_cola(nucleo->cola_nuevos)==false){
			//Existe alguno en la cola de nuevos
			Pcb* pcb_posible = get_cabeza(nucleo->cola_nuevos);
			pthread_mutex_lock(&nucleo->sem_memoria);
			int dato_enviar = 106;
			escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
			escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&pcb_posible->PID,sizeof(int));
			int respuesta;
			leer_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&respuesta,sizeof(int));
			if(respuesta == 107){
				Pcb* pcb = sacar_cabeza(nucleo->cola_nuevos);
				agregar_pcb(nucleo->cola_listos,pcb);
			}
			else{
				//Elimino el PID
				Pcb* pcb_a_borrar = sacar_pcb(nucleo->cola_nuevos,pcb_posible->PID);
				agregar_pcb(nucleo->cola_finalizados,pcb_a_borrar);
				printf("PCB movido a FINALIZADOS por no tener marcos libres");
				pcb_a_borrar = sacar_pcb(nucleo->cola_finalizados,pcb_a_borrar->PID);
				Consola* consola = buscar_consola_pid(nucleo->consolas,pcb_a_borrar->PID);
				avisar_a_consola_fin_programa_x_frames(nucleo,pcb_a_borrar->PID);
				destroy_pcb(pcb_a_borrar);
			}
			pthread_mutex_unlock(&nucleo->sem_memoria);
		}
		pthread_mutex_unlock(&nucleo->sem_pcb);
	}
}

void enviar_pcb(Nucleo* nucleo,Cpu* cpu,Pcb* pcb){
	pcb->QUANTUM = nucleo->QUANTUM;
	pcb->QUANTUM_SLEEP = nucleo->QUANTUM_SLEEP;
	int dato = 27; //Solicitud a CPU de enviar PCB
	printf("%d - Solicitud de envio de PCB a CPU\n",dato);
	printf("%d - Estado de cpu->osiosa\n",cpu->osiosa);
	printf("%d - Estado de cpu->ocupado\n",cpu->ocupado);
	escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
	int dato_leido;
	leer_descriptor(cpu->descriptor,(char*)&dato_leido,sizeof(int));
	if (dato_leido == 28){ //El CPU Notifico que esta listo para el PCB
		//Se envia el PCB
		printf("%d - Notifico de envio de PCB a CPU\n",dato_leido);
		printf("\nEnvio el PCB...\n");
		escribir_descriptor(cpu->descriptor,(char*)pcb,sizeof *pcb);
		escribir_descriptor(cpu->descriptor,(char*)pcb->INSTRUCCIONES,pcb->INSTRUCCIONES_SIZE * sizeof *pcb->INSTRUCCIONES);
		escribir_descriptor(cpu->descriptor,(char*)pcb->STACK,sizeof *pcb->STACK);
		printf("Se envia estructura ETIQUETAS de %d\n",pcb->ETIQUETAS_SIZE);
		escribir_descriptor(cpu->descriptor,(char*)pcb->ETIQUETAS,pcb->ETIQUETAS_SIZE);
		printf("Se envia estructura VARS de %d y de %d\n",pcb->STACK->VARS_SIZE,sizeof *pcb->STACK->VARS);
		escribir_descriptor(cpu->descriptor,(char*)pcb->STACK->VARS,pcb->STACK->VARS_SIZE * sizeof *pcb->STACK->VARS);
		escribir_descriptor(cpu->descriptor,(char*)pcb->STACK->RET_VAR,(pcb->STACK->POS +1) * sizeof *pcb->STACK->RET_VAR);
		escribir_descriptor(cpu->descriptor,(char*)pcb->STACK->RET_POS,(pcb->STACK->POS +1) * sizeof *pcb->STACK->RET_POS);
		printf("Se envio PCB que ocupa...%d\n",sizeof *pcb);
		printf("Se envio stack que ocupa...%d\n",sizeof *pcb->STACK);
		printf("STACK->POS...%d\n",pcb->STACK->POS);
		cpu->PID = pcb->PID;
	}
}

void hilo_para_memoria(Nucleo* nucleo){
	printf("Iniciando comunicacion con memoria\n");

	nucleo->DESCRIPTOR_MEMORIA = abrir_conexion(nucleo->IP_MEMORIA,nucleo->PUERTO_MEMORIA);

	printf("Comunicacion aceptada por la memoria en descriptor: %d\n",nucleo->DESCRIPTOR_MEMORIA);

	//int dato_leido;
	//leer_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&dato_leido,sizeof(int));
}

void hilo_para_cpus(Nucleo* nucleo){

	void nueva_cpu(int descriptor){
		printf("\nnueva cpu, descriptor: %d \n", descriptor);
		Cpu* cpu = new_cpu(descriptor);
		agregar_cpu(nucleo->cpus,cpu);
		NucleoCpu* nucleocpu = new_nucleocpu(nucleo,cpu);
		cpu->hilo_cpu = crear_hilo_un_para_un_cpu(hilo_para_un_cpu,nucleocpu);
	}

	void cerro_cpu(int descriptor){
		//no uso
		printf("\ncpu cerrada, descriptor: %d \n", descriptor);
	}

	int accion_cpu(int descriptor){
		//no uso
		return 1;
	}

	printf("\nIniciando Hilo de Consolas\n");

	int descriptor_servidor;

	char* puerto_cpu = string_itoa(nucleo->PUERTO_CPU);

	printf("Puerto cpu: %s \n",puerto_cpu);

	//Cerrar puerto si estaba abierto
	char* comando = string_new();
	string_append(&comando,"fuser -k ");
	string_append(&comando,puerto_cpu);
	string_append(&comando,"/tcp ");
	system(comando);

	descriptor_servidor = abrir_socket(puerto_cpu);

	if(descriptor_servidor == -1)
		printf("Error en abrir el Socket\n");
	else
		printf("Servidor Activo para Cpus Socket: %d \n", descriptor_servidor);

	iniciar_select(descriptor_servidor, nueva_cpu, accion_cpu, cerro_cpu);

	printf("\nFin del Select\n");
}

void hilo_para_un_cpu(NucleoCpu* nucleocpu){
	Nucleo* nucleo = nucleocpu->nucleo;
	Cpu* cpu = nucleocpu->cpu;
	destroy_nucleocpu(nucleocpu);

	int dato_enviar = 300;//dato sebo
	int byte_leidos;
	int bucle = 1;
	while(bucle){
		pthread_mutex_lock(&nucleo->sem_cpu);
		pthread_mutex_lock(&nucleo->sem_pcb);
		if(esta_vacia_la_cola(nucleo->cola_listos)==false){
			Pcb* pcb_listo = sacar_cabeza(nucleo->cola_listos);
			agregar_pcb(nucleo->cola_ejecutando,pcb_listo);
			enviar_pcb(nucleo,cpu,pcb_listo);
			if(nucleo->MONITOR_ACTIVADO)
				crear_hilo_un_para_un_monitor(hilo_monitor,nucleo);
			pthread_mutex_unlock(&nucleo->sem_pcb);
			cpu->osiosa = 0;
			pthread_mutex_unlock(&nucleo->sem_cpu);
			bucle = espera_ordenes_cpu(nucleo,cpu); //queda al servicio del cpu hasta que desaloje el PCB
		}
		pthread_mutex_unlock(&nucleo->sem_pcb);
		pthread_mutex_unlock(&nucleo->sem_cpu);
		usleep(1000000);
	}
}

void hilo_monitor(Nucleo* nucleo){
	mostrar_estados_rr(nucleo->FILE_MONITOR,nucleo->cola_nuevos,nucleo->cola_listos,nucleo->cola_ejecutando,nucleo->cola_bloqueados);
}

void borrar_pcb(Nucleo* nucleo, int PID){
	pthread_mutex_lock(&nucleo->sem_pcb);
	Pcb* pcb_a_borrar = sacar_pcb(nucleo->cola_ejecutando,PID);
	if (pcb_a_borrar != NULL){
		destroy_pcb(pcb_a_borrar);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		return;
	}
	pcb_a_borrar = sacar_pcb(nucleo->cola_listos,PID);
	if (pcb_a_borrar != NULL){
		destroy_pcb(pcb_a_borrar);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		return;
	}
	pcb_a_borrar = sacar_pcb(nucleo->cola_bloqueados,PID);
	if (pcb_a_borrar != NULL){
		destroy_pcb(pcb_a_borrar);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		return;
	}
	pthread_mutex_unlock(&nucleo->sem_pcb);
}

int espera_ordenes_cpu(Nucleo* nucleo, Cpu* cpu){
	int bucle = 1;
	int bytes_leidos;
	int orden;
	int dato;
	while(bucle){
		printf("\nQueda espera activa para CPU: descrip: %d\n",cpu->descriptor);
		bytes_leidos = leer_descriptor(cpu->descriptor,(char*)&orden,sizeof(int));
		if(bytes_leidos <= 0){
			//cpu desconectado
			printf("\nCPU CERRADO\n");
			if(cpu->PID != 0){
				//Significa que estaba ejecutando un proceso
				borrar_pcb(nucleo,cpu->PID);
				avisar_a_memoria_fin_programa(nucleo,cpu->PID);
				avisar_a_consola_fin_programa_x_aborto_cpu(nucleo,cpu->PID);
			}
			close(cpu->descriptor);
			printf("\nclose descriptor %d\n",cpu->descriptor);
			borrar_cpu(nucleo->cpus,cpu->descriptor);
			//pthread_exit(NULL);
			return 0; // salde del bucle mayor
		}
		else{
			//ejecutar accion del cpu
			if(orden == 43){//El CPU va a desalojar el PCB
				printf("\nId del Hilo: %d",process_get_thread_id());
				printf("\n%d - Solucitud CPU: desalojar PCB decrip CPU: %d\n",orden,cpu->descriptor);
				dato = 44; //Aviso al CPU que estoy listo para el PCB
				escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
				printf("\n%d - Respuesta CPU: desalojar PCB decrip CPU: %d\n",dato,cpu->descriptor);
				Pcb* pcb = recibir_pcb(nucleo,cpu);
				verificar_motivo_desalojo(nucleo,cpu,pcb);
				return 1; //sale de la funcion
			}
			bucle = realizar_una_orden_del_nucleo(nucleo,cpu,orden);
		}
	}
}

int realizar_una_orden_del_nucleo(Nucleo* nucleo, Cpu* cpu, int orden){
	int dato;
	if(orden == 532){//Forma abortiva
		printf("FORMA ABORTIVA DE CPU");
		if(cpu->PID != 0){
			//Significa que estaba ejecutando un proceso
			borrar_pcb(nucleo,cpu->PID);
			avisar_a_memoria_fin_programa(nucleo,cpu->PID);
			avisar_a_consola_fin_programa_x_aborto_cpu(nucleo,cpu->PID);
		}
		close(cpu->descriptor);
		borrar_cpu(nucleo->cpus,cpu->descriptor);
		pthread_exit(NULL);
	}
	if(orden == 39){//Se ejecuta wait
		//pthread_mutex_lock(&nucleo->sem_semaforos);
		printf("\n%d - Solucitud CPU: ejecutar wait Descrip CPU: %d\n",orden,cpu->descriptor);
		dato = 40; //Aviso al CPU que mando el id_Semaforo
		escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
		printf("\n%d - Respuesta CPU: ejecutar wait Descrip CPU: %d\n",dato,cpu->descriptor);
		int pid;
		leer_descriptor(cpu->descriptor,(char*)&pid,sizeof(int));
		char id_semaforo;
		leer_descriptor(cpu->descriptor,(char*)&id_semaforo,sizeof(char));
		printf("\nId de Semaforo recibido: %c pid: %d \n",id_semaforo,pid);
		int resultado_wait = ejecutar_wait(nucleo,cpu,id_semaforo,pid);
		if (resultado_wait != 1){
			escribir_descriptor(cpu->descriptor,(char*)&resultado_wait,sizeof(int));
			return 1;
		}
		//pthread_mutex_unlock(&nucleo->sem_semaforos);
		return 0;
	}
	if(orden == 47){//Se ejecuta signal
		//pthread_mutex_lock(&nucleo->sem_semaforos);
		printf("\n%d - Solucitud CPU: ejecutar signal Descriptor de CPU: %d \n",orden,cpu->descriptor);
		dato = 48; //Aviso al CPU que mando el id_Semaforo
		escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
		char id_semaforo;
		leer_descriptor(cpu->descriptor,(char*)&id_semaforo,sizeof(char));
		printf("\nId de Semaforo recibido: %c \n",id_semaforo);
		int resultado_signal = ejecutar_signal(nucleo,id_semaforo);
		escribir_descriptor(cpu->descriptor,(char*)&resultado_signal,sizeof(int));
		//pthread_mutex_unlock(&nucleo->sem_semaforos);
		return 1;
	}
	if(orden == 45){//El CPU solicita valor de Variable Global
		pthread_mutex_lock(&nucleo->sem_variables_glo);
		printf("\nId del Hilo: %d",process_get_thread_id());
		printf("\n%d - Solucitud CPU: Valor de Variable Global Desc CPU: %d \n",orden,cpu->descriptor);
		dato = 46; //Aviso al CPU que estoy listo para que envie la variable global
		escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
		printf("\n%d - Respuesta CPU: Valor de Variable Global \n",dato);
		int size_variable_global;
		leer_descriptor(cpu->descriptor,(char*)&size_variable_global,sizeof(int));
		printf("Se recibio un size de la variable global de %d\n",size_variable_global);
		//char* variable_global = malloc(size_variable_global);
		char* variable_global = calloc(size_variable_global,sizeof(char));
		leer_descriptor(cpu->descriptor,(char*)variable_global,size_variable_global);
		//Tengo que hacer eso porque trae basura despues de el id de variable
		//char* variable_arreglada = string_substring(variable_global,0,size_variable_global);
		printf("%s - Variable Global recibida\n",variable_global);
		int valor_variable_global = leer_variable_global(nucleo,variable_global);
		escribir_descriptor(cpu->descriptor,(char*)&valor_variable_global,sizeof(int));
		pthread_mutex_unlock(&nucleo->sem_variables_glo);
		return 1;
	}
	if(orden == 50){//El CPU solicita modificar una Variable Global
		pthread_mutex_lock(&nucleo->sem_variables_glo);
		printf("\n%d - Solucitud CPU: Modificar Variable Global \n",orden);
		dato = 51; //Aviso al CPU que estoy listo para que envie la variable global y valor
		escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
		printf("\n%d - Respuesta CPU: Modificar Variable Global \n",dato);
		int size_variable_global;
		leer_descriptor(cpu->descriptor,(char*)&size_variable_global,sizeof(int));
		printf("Se recibio un size de la variable global de %d\n",size_variable_global);
		char* id_variable_global = calloc(size_variable_global,sizeof(char));
		leer_descriptor(cpu->descriptor,(char*)id_variable_global,size_variable_global);
		printf("Variable Global recibida %s\n",id_variable_global);
		int nuevo_valor;
		leer_descriptor(cpu->descriptor,(char*)&nuevo_valor,sizeof(int));
		modificar_variable_global(nucleo,id_variable_global,nuevo_valor);
		pthread_mutex_unlock(&nucleo->sem_variables_glo);
		return 1;
	}
	if(orden == 52){//El CPU solicita imprimir un valor
		//cpu->ocupado = 1;
		printf("\n%d - Solucitud CPU: Imprimir valor \n",orden);
		dato = 53; //Aviso al CPU que estoy listo para que el valor
		escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
		printf("\n%d - Respuesta CPU: Imprimir \n",dato);
		int valor;
		leer_descriptor(cpu->descriptor,(char*)&valor,sizeof(int));
		int pid;
		leer_descriptor(cpu->descriptor,(char*)&pid,sizeof(int));
		printf("Se recibio el valor de %d para imprimir en PID: %d\n",valor,pid);
		imprimir_valor_en_consola(nucleo,valor,pid);
		return 1;
	}
	if(orden == 60){//El CPU solicita imprimir un texto
		printf("\n%d - Solucitud CPU: Imprimir texto \n",orden);
		dato = 61; //Aviso al CPU que estoy listo para que envie size y texto
		escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
		printf("\n%d - Respuesta CPU: Imprimir texto \n",dato);
		int size_cadena;
		leer_descriptor(cpu->descriptor,(char*)&size_cadena,sizeof(int));
		//char* cadena = malloc(size_cadena);
		char* cadena = calloc(size_cadena,sizeof(char));
		leer_descriptor(cpu->descriptor,(char*)cadena,size_cadena);
		int pid;
		leer_descriptor(cpu->descriptor,(char*)&pid,sizeof(int));
		printf("Se recibio el texto: %s de %d bytes para imprimir en PID: %d\n",cadena,size_cadena,pid);
		imprimir_texto_en_consola(nucleo,cadena,pid);
		return 1;
	}
	if(orden == 58){//El CPU solicita ejecutar IO
		printf("\n%d - Solucitud CPU: ejecutar IO \n",orden);
		dato = 59; //Aviso al CPU que mando el id_io y el valor
		escribir_descriptor(cpu->descriptor,(char*)&dato,sizeof(int));
		int pid;
		int size_io;
		char* io_id;
		int cantidad;
		leer_descriptor(cpu->descriptor,(char*)&pid,sizeof(int));
		leer_descriptor(cpu->descriptor,(char*)&size_io,sizeof(int));
		io_id = calloc(size_io,sizeof(char));
		leer_descriptor(cpu->descriptor,(char*)io_id,size_io);
		leer_descriptor(cpu->descriptor,(char*)&cantidad,sizeof(int));
		printf("\nId de IO recibido: %s pid: %d cantidad: %d \n",io_id,pid,cantidad);
		//Lo agrego a la lista de bloqueados.
		Pidblock* PCB_bloqueado = new_pidblock_io(pid,io_id,cantidad);
		agregar_pidblock(nucleo->lista_bloqueados,PCB_bloqueado);
		return 1;
	}
	return 1;
}

void modificar_variable_global(Nucleo* nucleo,char* id_variable,int valor){
	Variable* variable = buscar_variable(nucleo->variables,id_variable);
	pthread_mutex_lock(&variable->MUTEX);
	variable->valor_variable = valor;
	pthread_mutex_unlock(&variable->MUTEX);
	printf("Se modifico la variable global %s con el valor de %d \n",variable->id_variable,variable->valor_variable);
}

int leer_variable_global(Nucleo* nucleo,char* id_variable){
	Variable* variable = buscar_variable(nucleo->variables,id_variable);
	printf("Se envio el valor de %d \n",variable->valor_variable);
	pthread_mutex_lock(&variable->MUTEX);
	int valor_variable = variable->valor_variable;
	pthread_mutex_unlock(&variable->MUTEX);
	return valor_variable;
}

void hilo_para_un_io(NucleoIo* nucleoio){
	Nucleo* nucleo = nucleoio->nucleo;
	Io* io = nucleoio->io;
	Pidblock* pidblock = nucleoio->pidblock;
	destroy_nucleoio(nucleoio);

	//Si el IO esta siendo usado en ese momento se bloquea el hilo.
	pthread_mutex_lock(&io->MUTEX);
	int tiempo_total_retardo = (io->io_sleep * pidblock->CANTIDAD_IO) * 10000; //por 10000 xq el valor esta en mili segundos
	printf("\nSe ejecuta en hilo %s con cantidad de %d con tiempo total %d\n",io->io_id,pidblock->CANTIDAD_IO,tiempo_total_retardo);
	usleep(tiempo_total_retardo);
	pthread_mutex_unlock(&io->MUTEX);
	//Cuando termina pasa a listos
	pthread_mutex_lock(&nucleo->sem_pcb);
	Pcb* pcb_a_mover = sacar_pcb(nucleo->cola_bloqueados,pidblock->PID);
	agregar_pcb(nucleo->cola_listos,pcb_a_mover);
	pthread_mutex_unlock(&nucleo->sem_pcb);
}

void imprimir_valor_en_consola(Nucleo* nucleo,int valor,int pid){
	Consola* consola = buscar_consola_pid(nucleo->consolas,pid);
	int dato_enviar = 54;
	int valor_pid = pid;
	int dato = valor;
	if (consola->descriptor != -1){
		escribir_descriptor(consola->descriptor,(char*)&dato_enviar,sizeof(int));
		escribir_descriptor(consola->descriptor,(char*)&valor_pid,sizeof(int));
		escribir_descriptor(consola->descriptor,(char*)&dato,sizeof(int));
		printf("%d - Ordenar a consola que imprima valor PID: %d Valor: %d\n",dato_enviar,valor_pid,dato);
	}
}

void imprimir_texto_en_consola(Nucleo* nucleo,char* cadena,int pid){
	Consola* consola = buscar_consola_pid(nucleo->consolas,pid);
	int dato_enviar = 62;
	int valor_pid = pid;
	int size_cadena = string_length(cadena);
	if (consola->descriptor != -1){
		escribir_descriptor(consola->descriptor,(char*)&dato_enviar,sizeof(int));
		escribir_descriptor(consola->descriptor,(char*)&valor_pid,sizeof(int));
		escribir_descriptor(consola->descriptor,(char*)&size_cadena,sizeof(int));
		escribir_descriptor(consola->descriptor,(char*)cadena,size_cadena);
		printf("%d - Ordenar a consola que imprima texto PID: %d Texto: %s Size: %d\n",dato_enviar,valor_pid,cadena,size_cadena);
	}
}

void desbloquear_pcb_semaforo(NucleoSemaforo* nucleosemaforo){
	Nucleo* nucleo = nucleosemaforo->nucleo;
	Semaforo* semaforo = nucleosemaforo->semaforo;
	destroy_nucleosemaforo(nucleosemaforo);
	int bloqueado = 1;
	while (bloqueado){
		Pcb* pcb = buscar_pcb_bloqueado_semaforo(nucleo->cola_bloqueados,BLOQUEO_WAIT,semaforo->id_semaforo);
		if (pcb != NULL){
			Pcb* pcb_sacar = sacar_pcb(nucleo->cola_bloqueados,pcb->PID);
			agregar_pcb(nucleo->cola_listos,pcb_sacar);
			bloqueado = 0;
		}
		usleep(1000000); //consultar cada un segundo hasta que lo desbloquee;
	}
	//se auto destruye el hilo
}

int ejecutar_signal(Nucleo* nucleo,char id_semaforo){
	pthread_mutex_lock(&nucleo->sem_semaforos);
	Semaforo* semaforo = buscar_semaforo(nucleo->semaforos,id_semaforo);
	if (semaforo->valor_semaforo < 0){
		//Significa que debo desbloquear un PCB... pero no puedo bloquear al
		//PCB actual, entonces creo un hilo que se encargue de eso.
		NucleoSemaforo* nucleosemaforo = new_nucleosemaforo(nucleo,semaforo);
		crear_hilo_un_para_un_semaforo(desbloquear_pcb_semaforo,nucleosemaforo);
	}
	pthread_mutex_lock(&semaforo->MUTEX);
	semaforo->valor_semaforo = semaforo->valor_semaforo + 1;
	printf("Signal ejecutado id_sem: %c valor: %d",semaforo->id_semaforo,semaforo->valor_semaforo);
	pthread_mutex_unlock(&semaforo->MUTEX);
	pthread_mutex_unlock(&nucleo->sem_semaforos);
	return 49; //Avisar que termina semaforo
}

int ejecutar_wait(Nucleo* nucleo,Cpu* cpu, char id_semaforo,int pid){
	pthread_mutex_lock(&nucleo->sem_semaforos);
	Semaforo* semaforo = buscar_semaforo(nucleo->semaforos,id_semaforo);
	pthread_mutex_lock(&semaforo->MUTEX);
	semaforo->valor_semaforo = semaforo->valor_semaforo - 1;
	if (semaforo->valor_semaforo < 0){
		printf("\nBloquear Proceso\n");
		//Agrego en la lista de espera de bloqueados
		//coloco el pid y el semaforo que espera
		//Luego el PCB se va a ir a bloqueados
		//Pidblock* pidblock = new_pidblock(pid,id_semaforo);
		//agregar_pidblock(nucleo->lista_bloqueados,pidblock);

		pthread_mutex_unlock(&semaforo->MUTEX);
		pthread_mutex_unlock(&nucleo->sem_semaforos);
		return 41;//Bloquear Proceso
	}
	else{
		printf("\nNO Bloquear Proceso\n");
		pthread_mutex_unlock(&semaforo->MUTEX);
		pthread_mutex_unlock(&nucleo->sem_semaforos);
		return 42;//No Bloquear Proceso
	}
}

Pcb* recibir_pcb(Nucleo* nucleo,Cpu* cpu){
	Pcb* pcb = new_pcb();
	leer_descriptor(cpu->descriptor,(char*)pcb,sizeof *pcb);
	printf("Se recibio estructura PCB que ocupa %d\n",sizeof *pcb);
	pcb->INSTRUCCIONES = malloc(pcb->INSTRUCCIONES_SIZE * sizeof *pcb->INSTRUCCIONES);
	printf("Se recibira estructura INSTRUCCIONES de %d y de %d \n",pcb->INSTRUCCIONES_SIZE,sizeof *pcb->INSTRUCCIONES);
	leer_descriptor(cpu->descriptor,(char*)pcb->INSTRUCCIONES,pcb->INSTRUCCIONES_SIZE * sizeof *pcb->INSTRUCCIONES);
	printf("Se recibio estructura INSTRUCCIONES \n");
	pcb->STACK = malloc(sizeof *pcb->STACK);
	leer_descriptor(cpu->descriptor,(char*)pcb->STACK,sizeof *pcb->STACK);
	printf("Se recibira estructura ETIQUETAS de %d\n",pcb->ETIQUETAS_SIZE);
	pcb->ETIQUETAS = malloc(pcb->ETIQUETAS_SIZE);
	leer_descriptor(cpu->descriptor,(char*)pcb->ETIQUETAS,pcb->ETIQUETAS_SIZE);
	printf("Se recibira estructura VARS de %d y de %d \n",pcb->STACK->VARS_SIZE,sizeof *pcb->STACK->VARS);
	pcb->STACK->VARS = malloc(pcb->STACK->VARS_SIZE * sizeof *pcb->STACK->VARS);
	leer_descriptor(cpu->descriptor,(char*)pcb->STACK->VARS,pcb->STACK->VARS_SIZE * sizeof *pcb->STACK->VARS);
	pcb->STACK->RET_VAR = malloc((pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_VAR);
	leer_descriptor(cpu->descriptor,(char*)pcb->STACK->RET_VAR,(pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_VAR);
	pcb->STACK->RET_POS = malloc((pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_POS);
	leer_descriptor(cpu->descriptor,(char*)pcb->STACK->RET_POS,(pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_POS);
	printf("Se recibio el PCB desde el CPU \n");
	printf("%d - El ESTADO FINAL desalojo \n",pcb->ESTADO_FINAL);

	cpu->PID = 0; //el CPU no tiene PCB

	//Aviso a la memoria que debe borrar la TLB
	avisar_memoria_borrar_tlb_x_pid(nucleo,pcb->PID);

	return pcb;
}

void avisar_memoria_borrar_tlb_x_pid(Nucleo* nucleo,int PID){
	pthread_mutex_lock(&nucleo->sem_memoria);
	int dato_enviar = 56;
	escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&PID,sizeof(int));
	pthread_mutex_unlock(&nucleo->sem_memoria);
}

void verificar_motivo_desalojo(Nucleo* nucleo,Cpu* cpu,Pcb* pcb){
	if (pcb->ESTADO_FINAL == QUANTUM_COMPLETO){
		//Quitar PCB de ejecutandos y agregarlo en listos
		pthread_mutex_lock(&nucleo->sem_pcb);
		Pcb* pcb_a_borrar = sacar_pcb(nucleo->cola_ejecutando,pcb->PID);
		destroy_pcb(pcb_a_borrar);
		agregar_pcb(nucleo->cola_listos,pcb);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		//crear_hilo_un_parametro(hilo_mostrar_estados,nucleo);
		printf("\nSe movio PCB de ejecutando a Listos PID: %d \n",pcb->PID);
	}
	if(pcb->ESTADO_FINAL == BLOQUEO_WAIT){
		//Quitar PCB de ejecutandos y agregarlo en bloqueados
		//pthread_mutex_lock(&nucleo->sem_semaforos);
		pthread_mutex_lock(&nucleo->sem_pcb);
		Pcb* pcb_a_borrar = sacar_pcb(nucleo->cola_ejecutando,pcb->PID);
		destroy_pcb(pcb_a_borrar);
		agregar_pcb(nucleo->cola_bloqueados,pcb);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		//crear_hilo_un_parametro(hilo_mostrar_estados,nucleo);
		printf("\nSe movio PCB de ejecutando a Bloqueados PID por semaforo, PID: %d \n",pcb->PID);
		//pthread_mutex_unlock(&nucleo->sem_semaforos);
	}
	if(pcb->ESTADO_FINAL == BLOQUEO_IO){
		//Quitar PCB de ejecutandos y agregarlo en bloqueados
		pthread_mutex_lock(&nucleo->sem_pcb);
		Pcb* pcb_a_borrar = sacar_pcb(nucleo->cola_ejecutando,pcb->PID);
		destroy_pcb(pcb_a_borrar);
		agregar_pcb(nucleo->cola_bloqueados,pcb);
		//crear_hilo_un_parametro(hilo_mostrar_estados,nucleo);
		printf("\nSe movio PCB de ejecutando a Bloqueados por IO PID: %d \n",pcb->PID);
		Pidblock* PCB_bloqueado = buscar_y_sacar_pidblock_io(nucleo->lista_bloqueados,pcb->PID);
		Io* io = buscar_io(nucleo->ios,PCB_bloqueado->IO);
		NucleoIo* nucleoio = new_nucleoio(nucleo,io,PCB_bloqueado);
		//No puedo bloquearme aqui en el hilo del CPU entonces creao un hilo dedicado a eso
		crear_hilo_un_para_un_io(hilo_para_un_io,nucleoio);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		//Si dos procesos quieren ejecutar el mismo IO, se quedara a la espera
	}
	if(pcb->ESTADO_FINAL == SEGMENTO_FAULT){
		//Quitar PCB de ejecutandos y agregarlos en terminados
		pthread_mutex_lock(&nucleo->sem_pcb);
		Pcb* pcb_a_borrar = sacar_pcb(nucleo->cola_ejecutando,pcb->PID);
		destroy_pcb(pcb_a_borrar);
		agregar_pcb(nucleo->cola_finalizados,pcb);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		printf("\nSe movio PCB de ejecutando a Finalizados por SEGMENTO FAULT PID: %d \n",pcb->PID);
		avisar_a_memoria_fin_programa(nucleo,pcb->PID);
		avisar_a_consola_fin_programa(nucleo,pcb->PID);
	}
	if(pcb->ESTADO_FINAL == FINAL_PROGRAMA){
		//Quitar PCB de ejecutandos y agregarlos en terminados
		pthread_mutex_lock(&nucleo->sem_pcb);
		Pcb* pcb_a_borrar = sacar_pcb(nucleo->cola_ejecutando,pcb->PID);
		destroy_pcb(pcb_a_borrar);
		agregar_pcb(nucleo->cola_finalizados,pcb);
		pthread_mutex_unlock(&nucleo->sem_pcb);
		printf("\nSe movio PCB de ejecutando a Finalizados PID: %d \n",pcb->PID);
		avisar_a_memoria_fin_programa(nucleo,pcb->PID);
		avisar_a_consola_fin_programa(nucleo,pcb->PID);
	}
	if (pcb->FLAG_SISGUR1 == 1){
		//significa que el CPU de destruira despues que envio el PCB
		pcb->FLAG_SISGUR1 = 0; //vuelvo el FLAG a la normalidad
		printf("\nSE DESTRUYE EL CPU\n");
		int dato_enviar = 200;//kill the proccess
		escribir_descriptor(cpu->descriptor,(char*)&dato_enviar,sizeof(int));
		borrar_cpu(nucleo->cpus,cpu->descriptor);
		pthread_exit(NULL);
		//cpu->evitar_kill_pid = 1; //marco para evitar la destruccion.
	}
	else{
		//de lo contrario se pone OSIOSA
		cpu->osiosa = 1;
	}
}

void avisar_a_consola_fin_programa_x_aborto_cpu(Nucleo* nucleo,int pid){
	int dato_enviar = 788;
	int valor_pid = pid;
	Consola* consola = buscar_consola_pid(nucleo->consolas,valor_pid);
	escribir_descriptor(consola->descriptor,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(consola->descriptor,(char*)&valor_pid,sizeof(int));
	printf("\n%d - Avisar a consola que finalizo un programa PID: %d\n",dato_enviar,valor_pid);
	close(consola->descriptor);
	borrar_consola(nucleo->consolas,consola->descriptor);
}

void avisar_a_consola_fin_programa(Nucleo* nucleo,int pid){
	int dato_enviar = 65;
	int valor_pid = pid;
	Consola* consola = buscar_consola_pid(nucleo->consolas,valor_pid);
	escribir_descriptor(consola->descriptor,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(consola->descriptor,(char*)&valor_pid,sizeof(int));
	printf("\n%d - Avisar a consola que finalizo un programa PID: %d\n",dato_enviar,valor_pid);
	close(consola->descriptor);
	borrar_consola(nucleo->consolas,consola->descriptor);
}

void avisar_a_consola_fin_programa_x_frames(Nucleo* nucleo,int pid){
	int dato_enviar = 733;
	int valor_pid = pid;
	Consola* consola = buscar_consola_pid(nucleo->consolas,valor_pid);
	escribir_descriptor(consola->descriptor,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(consola->descriptor,(char*)&valor_pid,sizeof(int));
	printf("\n%d - Avisar a consola que finalizo un programa PID: %d x falta de frames\n",dato_enviar,valor_pid);
	close(consola->descriptor);
	borrar_consola(nucleo->consolas,consola->descriptor);
}

void avisar_a_memoria_fin_programa(Nucleo* nucleo,int pid){
	pthread_mutex_lock(&nucleo->sem_memoria);
	int dato_enviar = 55;
	int valor_pid = pid;
	escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(nucleo->DESCRIPTOR_MEMORIA,(char*)&valor_pid,sizeof(int));
	pthread_mutex_unlock(&nucleo->sem_memoria);
	printf("\n%d - Avisar a memoria que finalizo un programa PID: %d\n",dato_enviar,valor_pid);
}

void hilo_para_consolas(Nucleo* nucleo){

	void nueva_consola(int descriptor){
		printf("\nnuevo consola, descriptor: %d \n", descriptor);
		Consola* consola = new_consola(descriptor);
		agregar_consola(nucleo->consolas,consola);
		NucleoConsola* nucleoconsola = new_nucleoconsola(nucleo,consola);
		consola->hilo_consola = crear_hilo_un_para_una_consola(hilo_para_una_consola,nucleoconsola);
	}

	void cerro_consola(int descriptor){
		printf("\nconsola cerrada, descriptor: %d \n", descriptor);
	}

	int accion_consola(int descriptor){
		int resultado_lectura;
		int numero_leido;

		resultado_lectura = leer_descriptor(descriptor,(char*)&numero_leido,sizeof(int));
		if(resultado_lectura == 0){
			//Se va para cerro conexion
			return resultado_lectura;
		}
		else{
			printf("\nSe recibio, descriptor: %d valor: %d \n", descriptor,numero_leido);
			return resultado_lectura;
		}
	}

	printf("\nIniciando Hilo de Consolas\n");

	int descriptor_servidor;

	char* puerto_consola = string_itoa(nucleo->PUERTO_PROG);

	printf("Puerto consola: %s \n",puerto_consola);

	//Cerrar puerto si estaba abierto
	char* comando = string_new();
	string_append(&comando,"fuser -k ");
	string_append(&comando,puerto_consola);
	string_append(&comando,"/tcp ");
	system(comando);

	descriptor_servidor = abrir_socket(puerto_consola);

	if(descriptor_servidor == -1)
		printf("Error en abrir el Socket\n");
	else
		printf("Servidor Activo para Consolas Socket: %d \n", descriptor_servidor);

	iniciar_select(descriptor_servidor, nueva_consola, accion_consola, cerro_consola);

	printf("\nFin del Select\n");
}
