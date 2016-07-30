#include "memoria.h"
#include <stdlib.h>
#include <commons/config.h>
#include <pthread.h>
#include "servidor.h"
#include "ansisop.h"
#include "direccion.h"
#include "tlb.h"
#include "tlbs.h"

#define PATH_DUMP_ALL "dump-all.log" //Archivo de comando
#define PATH_DUMP_ONE "dump-one.log" //Archivo de comando
#define PATH_SHOW_ALL "show-all.log" //Archivo de comando

Memoria* new_memoria(){
	Memoria * m;
	m = malloc(sizeof(struct memoria));
	m->cpus = new_cpus();
	m->programas = new_programas();
	m->marcos = new_marcos();
	m->tlbs = new_tlbs();
	m->MONITOR_ACTIVADO = 0;
	m->interprete = new_interprete();

	m->FILE_DUMP_ALL = new_log(PATH_DUMP_ALL);
	m->FILE_DUMP_ONE = new_log(PATH_DUMP_ONE);
	m->FILE_SHOW_ALL = new_log(PATH_SHOW_ALL);

	//SEMAFOROS
	pthread_mutex_init(&m->sem_swap, NULL);
	pthread_mutex_init(&m->sem_listas, NULL);

	//m->FILE_MEM = new_log(PATH_LOG_MEM);
	//m->FILE_REG = new_log(PATH_LOG_FILE);
	return m;
}

MemoriaCpu* new_memoriacpu(Memoria* memoria,Cpu* cpu){
	MemoriaCpu * nc;
	nc = malloc(sizeof(struct memoriacpu));
	nc->memoria = memoria;
	nc->cpu = cpu;
	return nc;
}

//Destructor
void destroy_memoria(Memoria* memoria){
	free(memoria);
}

void destroy_memoriacpu(MemoriaCpu* memoriacou){
	free(memoriacou);
}

void set_path_archivo_configuracion(Memoria* memoria,char* path){
	memoria->path_conf_memoria = path;
}

void cargar_archivo_configuracion(Memoria* memoria){
	//Inicio del Nucleo - Cargar archivo
	t_config* config_file = config_create(memoria->path_conf_memoria);
	memoria->PUERTO = config_get_string_value(config_file,"PUERTO");
	memoria->PUERTO_NUCLEO = config_get_string_value(config_file,"PUERTO_NUCLEO");
	memoria->IP_SWAP = config_get_string_value(config_file,"IP_SWAP");
	memoria->PUERTO_SWAP = config_get_string_value(config_file,"PUERTO_SWAP");
	memoria->MARCO_SIZE = config_get_int_value(config_file,"MARCO_SIZE");
	memoria->MARCOS = config_get_int_value(config_file,"MARCOS");
	memoria->MARCO_X_PROC = config_get_int_value(config_file,"MARCO_X_PROC");
	char* ALGORTIRMO = config_get_string_value(config_file,"ALGORITMO");
	if (string_equals_ignore_case(ALGORTIRMO,"CLOCK")){
		memoria->ALGORITMO = 1;
	}else{
		memoria->ALGORITMO = 2;
	}
	memoria->RETARDO = 1000 * config_get_int_value(config_file,"RETARDO");
	memoria->ENTRADAS_TBL = config_get_int_value(config_file,"ENTRADAS_TBL");
	//printf("Parametros cargados desde el archivo confi \n");
}

void memoria_inciar(Memoria* memoria){
	memoria->hilo_para_nucleo = crear_hilo_un_parametro(hilo_para_nucleo,memoria);
	memoria->hilo_para_swap = crear_hilo_un_parametro(hilo_para_swap,memoria);
	memoria->hilo_para_cpus = crear_hilo_un_parametro(hilo_para_cpus,memoria);
	memoria->hilo_para_interprete = crear_hilo_un_parametro(hilo_para_interprete,memoria);

	//Esperar a que terminen los hilos creados
	pthread_join(memoria->hilo_para_nucleo, NULL);
	pthread_join(memoria->hilo_para_swap, NULL);
	pthread_join(memoria->hilo_para_cpus, NULL);
	//printf("\nFin de Nucleo\n");
}

void hilo_para_interprete(Memoria* memoria){
	Interprete* interprete = memoria->interprete;

	int bucle = 1;
	while(bucle){
		bucle = iniciar_interprete(interprete); //bloqueante

		if(interprete->comando_correcto){
			bucle = ejecutar_comando(memoria,interprete);
		}

	}
	destroy_interprete(interprete);
}

int ejecutar_comando(Memoria* memoria, Interprete* interprete){
	char* comando = dictionary_get(interprete->comandos,interprete->commandos_ingresados[0]);

	if (string_equals_ignore_case(comando,"comando_flush_tlb")){
		return comando_flush_tlb(memoria);
	}
	if (string_equals_ignore_case(comando,"comando_flush_memoria")){
		return comando_flush_memoria(memoria);
	}
	if (string_equals_ignore_case(comando,"comando_retardo")){
		return comando_retardo(memoria);
	}
	if (string_equals_ignore_case(comando,"comando_show_all")){
		return comando_show_all(memoria);
	}
	if (string_equals_ignore_case(comando,"comando_show_one")){
		return comando_show_one(memoria);
	}
	if (string_equals_ignore_case(comando,"comando_dump_all")){
		return comando_dump_all(memoria);
	}
	if (string_equals_ignore_case(comando,"comando_dump_one")){
		return comando_dump_one(memoria);
	}
	return 1;
}

int comando_dump_all(Memoria* memoria){
	pthread_mutex_lock(&memoria->sem_listas);
	printf("%s\n","TABLA ESTADO TLB:\n==================\nNRO TLB     PID      NRO PAGINA      NRO MARCO     REFERENCIADA");
	printf("%s\n",recorrer_tabla_tlbs(memoria->tlbs));
	printf("%s\n","TABLA ESTADO PROGRAMAS:\n=======================\nPID      CANT. MARCOS");
	printf("%s\n",recorrer_tabla_programas(memoria->programas));
	printf("%s\n","TABLA ESTADO MARCOS:\n====================\nNRO MARCO     PID      NRO PAGINA      USADA      MODIFICADA");
	printf("%s\n",recorrer_tabla_marcos(memoria->marcos));
	mostrar_estado_memoria(memoria->FILE_DUMP_ALL,memoria->tlbs,memoria->programas,memoria->marcos);//Crea archivo
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

int comando_dump_one(Memoria* memoria){
	Interprete* interprete = memoria->interprete;
	pthread_mutex_lock(&memoria->sem_listas);
	int PID = convertir_cadena_a_entero(interprete->commandos_ingresados[1]);
	printf("%s\n","TABLA ESTADO TLB:\n==================\nNRO TLB     PID      NRO PAGINA      NRO MARCO     REFERENCIADA");
	printf("%s\n",recorrer_tabla_tlbs_un_pid(memoria->tlbs,PID));
	printf("%s\n","TABLA ESTADO PROGRAMAS:\n=======================\nPID      CANT. MARCOS");
	printf("%s\n",recorrer_tabla_programas_un_pid(memoria->programas,PID));
	printf("%s\n","TABLA ESTADO MARCOS:\n====================\nNRO MARCO     PID      NRO PAGINA      USADA      MODIFICADA");
	printf("%s\n",recorrer_tabla_marcos_un_pid(memoria->marcos,PID));
	mostrar_estado_memoria_un_pid(memoria->FILE_DUMP_ONE,memoria->tlbs,memoria->programas,memoria->marcos,PID);//Crea archivo
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

int comando_show_one(Memoria* memoria){
	Interprete* interprete = memoria->interprete;
	pthread_mutex_lock(&memoria->sem_listas);
	Marcos* marcos_de_un_proceso = marcos_un_proceso(memoria->marcos,convertir_cadena_a_entero(interprete->commandos_ingresados[1]));
	int index;
	Marco* marco;
	for (index = 0; index < list_size(marcos_de_un_proceso->lista);index++){
		marco = list_get(marcos_de_un_proceso->lista,index);
		printf("\nPID: %d Id Marco: %d Contenido: %s\n",marco->PID, marco->MARCOID,memoria->VECTOR_MEMORIA[marco->MARCOID]);
	}
	destroy_marcos(marcos_de_un_proceso);
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

int comando_show_all(Memoria* memoria){
	pthread_mutex_lock(&memoria->sem_listas);
	Marcos* marcos = memoria->marcos;
	int index;
	for(index = 0; index < list_size(marcos->lista); index++){
		printf("\nId Marco: %d Contenido: %s\n",index,memoria->VECTOR_MEMORIA[index]);
	}
	//mostrar_contenido_all(memoria->FILE_SHOW_ALL,memoria->marcos,&memoria->VECTOR_MEMORIA);
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

int comando_retardo(Memoria* memoria){
	Interprete* interprete = memoria->interprete;
	pthread_mutex_lock(&memoria->sem_listas);
	memoria->RETARDO = 1000 * convertir_cadena_a_entero(interprete->commandos_ingresados[1]);
	printf("\nNuevo retardo: %d milisegundos\n",memoria->RETARDO / 1000);
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

int comando_flush_tlb(Memoria* memoria){
	pthread_mutex_lock(&memoria->sem_listas);
	if (memoria->ENTRADAS_TBL != 0){
		borrar_tlb_completa(memoria->tlbs);
	}
	printf("flush tlb successful!\n");
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

int comando_flush_tlb_x_pid(Memoria* memoria,int PID){
	pthread_mutex_lock(&memoria->sem_listas);
	if (memoria->ENTRADAS_TBL != 0){
		borrar_tlb_x_pid(memoria->tlbs,PID);
	}
	//printf("flush tlb successful!\n");
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

int comando_flush_memoria(Memoria* memoria){
	pthread_mutex_lock(&memoria->sem_listas);
	pasar_todas_modificado(memoria->marcos);
	printf("flush memory successful!\n");
	pthread_mutex_unlock(&memoria->sem_listas);
	return 1;
}

void hilo_para_cpus(Memoria* memoria){

	void nueva_cpu(int descriptor){
		//printf("\nnuevo cpu, descriptor: %d \n", descriptor);
		Cpu* cpu = new_cpu(descriptor);
		agregar_cpu(memoria->cpus,cpu);
		MemoriaCpu* memoriacpu = new_memoriacpu(memoria,cpu);
		cpu->hilo_para_cpu = crear_hilo_para_un_cpu(hilo_para_un_cpu,memoriacpu);
	}

	void cerro_cpu(int descriptor){
		//printf("\nconsola cerrada, descriptor: %d \n", descriptor);
	}

	int accion_cpu(int descriptor){
		int resultado_lectura;
		int numero_leido;

		resultado_lectura = leer_descriptor(descriptor,(char*)&numero_leido,sizeof(int));
		if(resultado_lectura == 0){
			//Se va para cerro conexion
			return resultado_lectura;
		}
		else{
			//printf("\nSe recibio, descriptor: %d valor: %d \n", descriptor,numero_leido);
			return resultado_lectura;
		}
	}

	//printf("\nIniciando Hilo de Cpus\n");

	int descriptor_servidor;

	//printf("Puerto cpus: %s \n",memoria->PUERTO);

	//Cerrar puerto si estaba abierto
	char* comando = string_new();
	string_append(&comando,"fuser -k ");
	string_append(&comando,memoria->PUERTO);
	string_append(&comando,"/tcp ");
	system(comando);

	descriptor_servidor = abrir_socket(memoria->PUERTO);

	if(descriptor_servidor == -1)
		printf("Error en abrir el Socket\n");
	else
		printf("Servidor Activo para Cpus Socket: %d \n", descriptor_servidor);

	iniciar_select(descriptor_servidor, nueva_cpu, accion_cpu, cerro_cpu);

	//printf("\nFin del Select\n");
}

void hilo_para_un_cpu(MemoriaCpu* memoriacpu){
	Memoria* memoria = memoriacpu->memoria;
	Cpu* cpu = memoriacpu->cpu;
	destroy_memoriacpu(memoriacpu);

	//printf("\nHilo para un cpu %d\n",cpu->descriptor);
	int leer_dato;
	int resultado;
	int cerro_conexion = 0;
	while(cerro_conexion == 0){
		resultado = leer_descriptor(cpu->descriptor,(char*)&leer_dato,sizeof(int));
		if(resultado <= 0){
			//printf("cpu cerrada");
			close(cpu->descriptor);
			borrar_cpu(memoria->cpus,cpu->descriptor);
			pthread_exit(NULL);
		}
		else{
			cerro_conexion = realizar_accion_cpu(memoria,cpu,leer_dato);
		}
	}
}

int realizar_accion_cpu(Memoria* memoria, Cpu* cpu, int accion){
	//printf("\nAccion: %d\n",accion);
	int dato_enviar;
	if(accion == 29){ //Solicitud de Direccion desde un CPU
		//printf("\n%d - Solucitud de direccion desde CPU descriptor: %d \n",accion,cpu->descriptor);
		dato_enviar = 30;  //Listo para recibir la Direccion, ocupo el descriptor del CPU por un rato
		escribir_descriptor(cpu->descriptor,(char*)&dato_enviar,sizeof(int));
		recibir_direccion_del_cpu(memoria, cpu);
		return 0;
	}
	if(accion == 35){//Solicitud de Direccion desde un CPU para almacenar
		//printf("\n%d - Solucitud de direccion desde CPU para almacenar \n",accion);
		int resultado_escritura;
		dato_enviar = 36;  //Listo para recibir la Direccion para almacenar, ocupo el descriptor del CPU por un rato
		escribir_descriptor(cpu->descriptor,(char*)&dato_enviar,sizeof(int));
		recibir_direccion_para_almacenar_del_cpu(memoria, cpu);
	}
	return 0;
}

void recibir_direccion_del_cpu(Memoria* memoria, Cpu* cpu){
	pthread_mutex_lock(&memoria->sem_listas);//Solo se lee o escribe marco por vez.
	//printf("Aqui recibo la direccion  \n");
	//save_log(memoria->FILE_REG,"Aqui recibo la direccion de lectura \n");
	Direccion* direccion = new_direccion_vacia();
	leer_descriptor(cpu->descriptor,(char*)direccion,sizeof *direccion);
	//printf("PID: %d\n",direccion->PID);
	//printf("PAG: %d\n",direccion->PAG);
	//printf("OFFSET %d\n",direccion->OFFSET);
	//printf("SIZE %d\n",direccion->SIZE);
	//save_log_int(memoria->FILE_REG,"PID: %d\n",direccion->PID);
	//save_log_int(memoria->FILE_REG,"PAG: %d\n",direccion->PAG);
	//save_log_int(memoria->FILE_REG,"OFFSET %d\n",direccion->OFFSET);
	//save_log_int(memoria->FILE_REG,"SIZE %d\n",direccion->SIZE);
	//save_log_int(memoria->FILE_REG,"BUFFER %d\n",direccion->BUFFER);

	//Verificar si la direccion estaba ya cargada en memoria, hacer fallo sino.
	char* cadena_out;
	int escribir = 0;
	int retardo = 1; //por defecto aplico retardo
	if (memoria->ENTRADAS_TBL == 0){//signficia que la TLB esta desactivada
		cadena_out = buscar_direccion_en_memoria(memoria,direccion,escribir);
	}
	else{
		cadena_out = buscar_direccion_en_tlb(memoria,direccion,escribir,&retardo);
	}

	if (retardo){
		//printf("\nRetardo aplicado: %d\n",retardo);
		//save_log_int(memoria->FILE_REG,"\nRetardo aplicado: %d\n",retardo);
		usleep(memoria->RETARDO);
	}
	//crear_hilo_un_parametro(hilo_mostrar_estado_memoria,memoria);		//muestra estado de memoria

	//printf("\nBuffer enviado a CPU: %s\n",cadena_out);
	//save_log_char(memoria->FILE_REG,"\nBuffer enviado a CPU: %s\n",cadena_total);

	if(memoria->MONITOR_ACTIVADO)
		crear_hilo_un_para_un_monitor(hilo_monitor,memoria);

	escribir_descriptor(cpu->descriptor,(char*)cadena_out,direccion->SIZE);
	pthread_mutex_unlock(&memoria->sem_listas);
}

void recibir_direccion_para_almacenar_del_cpu(Memoria* memoria,Cpu* cpu){
	pthread_mutex_lock(&memoria->sem_listas);//Solo se lee o escribe marco por vez.

	//printf("Aqui recibo la direccion  \n");
	//save_log(memoria->FILE_REG,"Aqui recibo la direccion  \n");
	Direccion* direccion = new_direccion_vacia();
	leer_descriptor(cpu->descriptor,(char*)direccion,sizeof *direccion);
	//printf("PID: %d\n",direccion->PID);
	//printf("PAG: %d\n",direccion->PAG);
	//printf("OFFSET %d\n",direccion->OFFSET);
	//printf("SIZE %d\n",direccion->SIZE);
	//printf("BUFFER %d\n",direccion->BUFFER);
	//save_log_int(memoria->FILE_REG,"PID: %d\n",direccion->PID);
	//save_log_int(memoria->FILE_REG,"PAG: %d\n",direccion->PAG);
	//save_log_int(memoria->FILE_REG,"OFFSET %d\n",direccion->OFFSET);
	//save_log_int(memoria->FILE_REG,"SIZE %d\n",direccion->SIZE);
	//save_log_int(memoria->FILE_REG,"BUFFER %d\n",direccion->BUFFER);

	//Verificar si la dirreccion estaba ya cargada en memoria, hacer fallo sino y escribirla
	char* cadena_out; //
	int escribir = 1; //seteo la funcion para escribir en memoria. (pasar a modificado el Marco)
	int retardo = 1; //por defecto aplico retardo
	if (memoria->ENTRADAS_TBL == 0){//signficia que la TLB esta desactivada
		cadena_out = buscar_direccion_en_memoria(memoria,direccion,escribir);
	}
	else{
		cadena_out = buscar_direccion_en_tlb(memoria,direccion,escribir,&retardo);
	}
	//crear_hilo_un_parametro(hilo_mostrar_estado_memoria,memoria);		//muestra estado de memoria
	if (retardo){
		usleep(memoria->RETARDO);
	}
	pthread_mutex_unlock(&memoria->sem_listas);
}

void hilo_para_swap(Memoria* memoria){
	memoria->DESCRIPTOR_SWAP = abrir_conexion(memoria->IP_SWAP,memoria->PUERTO_SWAP);

	if (memoria->DESCRIPTOR_SWAP == -1){
		//printf("\nErrod de conexion con swap\n");
	}
	else{
		//printf("\nConexion con swap establecida, descriptor: %d\n",memoria->DESCRIPTOR_SWAP);
	}
}

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Memoria*),Memoria* memoria){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (Memoria*)memoria);

	return thd1;
}

pthread_t crear_hilo_para_un_cpu(void(*funcion_externa)(MemoriaCpu*),MemoriaCpu* memoriacpu){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (MemoriaCpu*)memoriacpu);

	return thd1;
}

void hilo_monitor(Memoria* memoria){
	mostrar_estado_memoria(memoria->FILE_MONITOR,memoria->tlbs,memoria->programas,memoria->marcos);
}

void crear_hilo_un_para_un_monitor(void(*funcion_externa)(Memoria*),Memoria* memoria){
	pthread_attr_t atributos;
	pthread_attr_init(&atributos);
	pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_DETACHED); //Liberar recursos automaticamente
	pthread_t thd1;
	pthread_create (&thd1, &atributos, (void*)funcion_externa, (Memoria*)memoria);
}

void hilo_para_nucleo(Memoria* memoria){
	//printf("\nIniciando Hilo de Nucleo\n");

	int descriptor_servidor;

	//printf("Puerto nucleo: %s \n",memoria->PUERTO_NUCLEO);
	//save_log_char(memoria->FILE_REG,"Puerto nucleo: %s\n",memoria->PUERTO_NUCLEO);
	descriptor_servidor = abrir_socket(memoria->PUERTO_NUCLEO);

	if(descriptor_servidor == -1){
		//printf("\nError al abrir puerto para nucleo\n");
	}
	else{
		//printf("\nEsperando conexion del Nucleo, descriptor: %d\n",descriptor_servidor);
	}

	memoria->DESCRIPTOR_NUCLEO = aceptar_conexion_cliente(descriptor_servidor);

	//printf("Conexion establecida con Nucleo en Descriptor: %d\n",memoria->DESCRIPTOR_NUCLEO);
	//save_log_int(memoria->FILE_REG,"\nConexion establecida con Nucleo en Descriptor: %d\n",memoria->DESCRIPTOR_NUCLEO);

	int dato_leido;
	int resultado_lectura;

	int bucle = 1;
	while(bucle){
		resultado_lectura = leer_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)&dato_leido,sizeof(int));
		if (resultado_lectura <= 0){
			//printf("\nNucleo Cerrado\n");
			close(memoria->DESCRIPTOR_NUCLEO);
			bucle = 0;
		}
		else{
			bucle = realizar_una_accion_del_nucleo(memoria,dato_leido);

		}
	}
}

int realizar_una_accion_del_nucleo(Memoria* memoria, int dato_leido){
	if (dato_leido==23){
		recibir_size_ansisop(memoria);
		return 1;
	}
	if (dato_leido==55){//Nucleo avisa que se debe borrar un programa
		orden_borrar_programa(memoria);
		return 1;
	}
	if (dato_leido==56){//Nucleo avisa que se debe borrar un programa
		int PID;
		leer_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)&PID,sizeof(int)); //Ya me cuido de la concurrencia en el nucleo
		comando_flush_tlb_x_pid(memoria,PID);
		return 1;
	}
	if(dato_leido == 106){//Verificar si tiene frame libres
		int PID;
		leer_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)&PID,sizeof(int));
		pthread_mutex_lock(&memoria->sem_listas);
		int responder;
		if(cantidad_marcos_ocupados(memoria->marcos) < memoria->MARCOS){
			responder = 107;
		}
		else{
			responder = 108;
		}
		escribir_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)&responder,sizeof(int));
		pthread_mutex_unlock(&memoria->sem_listas);
		return 1;
	}
}

void orden_borrar_programa(Memoria* memoria){
	int pid;
	leer_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)&pid,sizeof(int));
	//printf("\n55 - Nucleo avisa que se debe borrar el PID: %d\n",pid);
	//save_log_int(memoria->FILE_REG,"\n55 - Nucleo avisa que se debe borrar el PID: %d\n",pid);
	//borrar_programas
	pthread_mutex_lock(&memoria->sem_listas);
	borrar_tlb_completa(memoria->tlbs);
	borrar_programa(memoria->programas,pid);
	borrar_marco(memoria->marcos,pid);
	pthread_mutex_unlock(&memoria->sem_listas);
	//crear_hilo_un_parametro(hilo_mostrar_estado_memoria,memoria);

	int dato_enviar = 57; //Avisar a Swap que borre el programa

	pthread_mutex_lock(&memoria->sem_swap);
	escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&pid,sizeof(int));
	pthread_mutex_unlock(&memoria->sem_swap);
}

void recibir_size_ansisop(Memoria* memoria){
	//no necesito semaforo.... esta controlado desde nucleo.
	Ansisop* ansisop = new_ansisop();
	leer_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)ansisop,sizeof *ansisop);
	ansisop->CADENA = malloc(ansisop->SIZE);
	leer_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)ansisop->CADENA,ansisop->SIZE);
	//printf("\nRecibi de Nucleo size ansisop %d\n",ansisop->SIZE);

	//printf("\n%s\n",ansisop->CADENA);

	//Puede que este usando el swap para resolver direcciones, uso mutex
	pthread_mutex_lock(&memoria->sem_swap);
	int dato_enviar = 25;
	escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&dato_enviar,sizeof(int));
	escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)ansisop,sizeof *ansisop);
	escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)ansisop->CADENA,ansisop->SIZE);

	int respuesta_swap;
	leer_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&respuesta_swap,sizeof(int));
	pthread_mutex_unlock(&memoria->sem_swap);
	escribir_descriptor(memoria->DESCRIPTOR_NUCLEO,(char*)&respuesta_swap,sizeof(int));
}

char* buscar_direccion_en_memoria(Memoria* memoria,Direccion* direccion,int escribir){
	Programa* programa;
	Marco* marco;
	char* cadena_out;
	programa = buscar_programa(memoria->programas,direccion->PID);
	if (programa == NULL){
		//save_log(memoria->FILE_REG,"\nPrograma no encontrado -> fallo de pagina\n");
		//printf("\nPrograma no encontrado -> fallo de pagina\n");
		//if (cantidad_programas(memoria->programas) < memoria->CANT_MAX_ANSISOP){
		if(cantidad_marcos_ocupados(memoria->marcos) < memoria->MARCOS){
			//tiene lugar en la memoria
			//Agregar registros a programas y marcos
			programa = new_programa(direccion->PID);
			programa->CANT_MARCOS++;
			agregar_programa(memoria->programas,programa);

			incorporar_marco(memoria,direccion,escribir);
			marco = buscar_marco(memoria->marcos,direccion->PID,direccion->PAG);

			programa->PUNTERO_CLOCK = marco->MARCOID;

			cadena_out = leer_marco(memoria,direccion,marco);
			//printf("Resultado de lectora: %s",cadena_out);
			//save_log_char(memoria->FILE_REG,"Resultado de lectora: %s",cadena_out);
			return cadena_out;
		}
		else{
			//printf("\nError - Nivel de multiprogramacion al maximo \n");
			return "no se puede ejecutar en Multiprogramacion";
		}
	}
	else{//Programa con paginas almacenadas
		//printf("\nPrograma encontrado -> Verificar si esta la pagina\n");
		//save_log(memoria->FILE_REG,"\nPrograma encontrado -> Verificar si esta la pagina\n");
		marco = buscar_marco(memoria->marcos,direccion->PID,direccion->PAG);
		if (marco == NULL){
			//No esta el marco pre-cargado en memoria-> fallo de pagina
			if(cantidad_marcos_ocupados(memoria->marcos) < memoria->MARCOS &&
					programa->CANT_MARCOS < memoria->MARCO_X_PROC){
				//tengo lugar para agregar un nuevo marco
				programa->CANT_MARCOS++;
				//printf("Pagina No encontrada -> Fallo de Pagina\n");
				//save_log(memoria->FILE_REG,"Pagina No encontrada -> Fallo de Pagina\n");
				incorporar_marco(memoria,direccion,escribir);

				//Test
				marco = buscar_marco(memoria->marcos,direccion->PID,direccion->PAG);
				cadena_out = leer_marco(memoria,direccion,marco);
				if(escribir){
					escribir_marco(memoria,direccion,marco);
					//printf("Pagina Modificada. \n");
					//save_log(memoria->FILE_REG,"Pagina Modificada. \n");
				}
				return cadena_out;
			}
			else{//Se debe elegir un marco para reemplazar
				//printf("\nSe hace reemplazo, algoritmo: %d\n",memoria->ALGORITMO);
				//save_log_int(memoria->FILE_REG,"\nSe hace reemplazo, algoritmo: %d\n",memoria->ALGORITMO);
				//if (memoria->ALGORITMO == 1){//Algoritmo Clock
					reemplazo_marco(memoria,direccion);
					//printf("\nSe reemplaza Marco con algoritmo de Clock\n");
					//save_log(memoria->FILE_REG,"\nSe reemplaza Marco con algoritmo de Clock\n");

					//Test
					//printf("\nEl nuevo marco a buscar: %d PAG: %d\n",direccion->PID,direccion->PAG);
					//save_log_int2(memoria->FILE_REG,"\nEl nuevo marco a buscar: %d PAG: %d\n",direccion->PID,direccion->PAG);
					marco = buscar_marco(memoria->marcos,direccion->PID,direccion->PAG);
					cadena_out = leer_marco(memoria,direccion,marco);
					if(escribir){
						escribir_marco(memoria,direccion,marco);
						//printf("Pagina Modificada. \n");
					}
					return cadena_out;
				//}
			}
		}
		else{
			//Marco encotrado, No se Falla de pagina.
			//printf("Pagina encontrada -> No se hace Fallo de Pagina\n");
			//save_log(memoria->FILE_REG,"Pagina encontrada -> No se hace Fallo de Pagina\n");
			cadena_out = leer_marco(memoria,direccion,marco);
			if(escribir){
				escribir_marco(memoria,direccion,marco);
				//printf("Pagina Modificada. \n");
				//save_log(memoria->FILE_REG,"Pagina Modificada. \n");
			}
			return cadena_out;
		}
	}

	return "error de pagina";
}

char* buscar_direccion_en_tlb(Memoria* memoria, Direccion* direccion, int escribir,int* pro_retardo){
	char* cadena_out;
	Tlb* tlb;
	Marco* marco;
	//Verificar si en la TLB esta la pagina
	tlb = buscar_tlb(memoria->tlbs,direccion->PAG,direccion->PID);
	if (tlb == NULL){
		//No esta, seteo que voy a aplicar un retardo
		*pro_retardo = 1;
	}
	else{
		*pro_retardo = 0;
	}

	cadena_out = buscar_direccion_en_memoria(memoria,direccion,escribir);

	//el marco ya va a estar cargado ahora en memoria
	marco = buscar_marco(memoria->marcos,direccion->PID,direccion->PAG);
	tlb = new_tlb(direccion->PID,direccion->PAG,marco->MARCOID);

	//Agregar entrada en la TLB (Si ya estaba, aumenta la referencia, sino aplica LRU)
	agregar_tlb(memoria->tlbs,tlb);

	return cadena_out;
}

void incorporar_marco(Memoria* memoria,Direccion* direccion,int escribir){
	Marco* marco = new_marco(memoria->marcos->MARCOID_ACTUAL,direccion->PID,direccion->PAG,escribir);
	agregar_marco(memoria->marcos,marco);
	memoria->VECTOR_MEMORIA[marco->MARCOID] = leer_pagina_swap(memoria,direccion);
}

void crear_estructuras_administrativas(Memoria* memoria){
	//Crear memoria
	memoria->VECTOR_MEMORIA = calloc(memoria->MARCOS,memoria->MARCO_SIZE);
	//printf("Memoria: %d",memoria->MARCO_SIZE);
	memoria->tlbs->CANT_ENTRADAS = memoria->ENTRADAS_TBL;
}

char* leer_marco(Memoria* memoria,Direccion* direccion,Marco* marco){
	//printf("\nLeer pid: %d pag: %d size: %d offset: %d marcoid: %d\n",direccion->PID,direccion->PAG,direccion->SIZE,direccion->OFFSET,marco->MARCOID);
	//char* cadena_instruccion;
	//save_log(memoria->FILE_REG,"\nLeer:\n");
	//save_log_int(memoria->FILE_REG,"PID: %d\n",direccion->PID);
	//save_log_int(memoria->FILE_REG,"PAG: %d\n",direccion->PAG);
	//save_log_int(memoria->FILE_REG,"OFFSET %d\n",direccion->OFFSET);
	//save_log_int(memoria->FILE_REG,"SIZE %d\n",direccion->SIZE);
	//save_log_int(memoria->FILE_REG,"BUFFER %d\n",direccion->BUFFER);
	//save_log_int(memoria->FILE_REG,"MARCOID: %d\n",marco->MARCOID);
	char* cadena_instruccion = malloc(direccion->SIZE);
	cadena_instruccion = string_substring(memoria->VECTOR_MEMORIA[marco->MARCOID],direccion->OFFSET,direccion->SIZE);
	marco->USADA = 1;
	return cadena_instruccion;
}

void escribir_marco(Memoria* memoria, Direccion* direccion, Marco* marco){
	//printf("\nEscribir marco: pid: %d page: %d size: %d offset: %d marcoid: %d buffer: %d\n",direccion->PID,direccion->PAG,direccion->SIZE,direccion->OFFSET,marco->MARCOID,direccion->BUFFER);
	//printf("\nLeer pid: %d pag: %d size: %d offset: %d marcoid: %d\n",direccion->PID,direccion->PAG,direccion->SIZE,direccion->OFFSET,marco->MARCOID);
	//char* cadena_instruccion;
	//save_log(memoria->FILE_REG,"\nEscribir:\n");
	//save_log_int(memoria->FILE_REG,"PID: %d\n",direccion->PID);
	//save_log_int(memoria->FILE_REG,"PAG: %d\n",direccion->PAG);
	//save_log_int(memoria->FILE_REG,"OFFSET %d\n",direccion->OFFSET);
	//save_log_int(memoria->FILE_REG,"SIZE %d\n",direccion->SIZE);
	//save_log_int(memoria->FILE_REG,"BUFFER %d\n",direccion->BUFFER);
	//save_log_int(memoria->FILE_REG,"PID: %d\n",marco->MARCOID);

	char* pagina_a_modificar;
	pagina_a_modificar = memoria->VECTOR_MEMORIA[marco->MARCOID];

	int buffer = direccion->BUFFER;
	char* variable = malloc(4);
	variable = (char*)&buffer;

	//se modifica la pagina Byte por Byte
	pagina_a_modificar[direccion->OFFSET + 0] = variable[0];
	pagina_a_modificar[direccion->OFFSET + 1] = variable[1];
	pagina_a_modificar[direccion->OFFSET + 2] = variable[2];
	pagina_a_modificar[direccion->OFFSET + 3] = variable[3];

	memoria->VECTOR_MEMORIA[marco->MARCOID] = pagina_a_modificar;

	marco->MODIFICADA = 1; //Marco que se modifico.
}

void reemplazo_marco(Memoria* memoria,Direccion* direccion){
	Programa* programa = buscar_programa(memoria->programas,direccion->PID);

	ordenar_idmarco(memoria->marcos);

	Marcos* marcos_de_un_proceso = marcos_un_proceso(memoria->marcos,direccion->PID);

	Marco* marco_a_reemplazar;
	if (memoria->ALGORITMO == 1){//es algoritmo clock
		marco_a_reemplazar = buscar_marco_reemplaza_clock(marcos_de_un_proceso,programa->PUNTERO_CLOCK);
	}
	else{
		//Sino es algoritmo de cleock modificado
		marco_a_reemplazar = buscar_marco_reemplaza_clock_modificado(marcos_de_un_proceso,direccion->PID,programa->PUNTERO_CLOCK);
	}

	programa->PUNTERO_CLOCK = obtener_nueva_posicion_puntero(marcos_de_un_proceso,marco_a_reemplazar);

	//printf("\nSe seleciono el marco a reemplazar idMArco: %d\n",marco_a_reemplazar->MARCOID);
	//save_log_int2(memoria->FILE_REG,"\nSe seleciono el marco a reemplazar idMArco: %d Pag: %d\n",marco_a_reemplazar->MARCOID,marco_a_reemplazar->PAGINAID);
	//printf("\nEstado de Marco modificado deberia: %d\n",marco_a_reemplazar->MODIFICADA);
	//save_log_int(memoria->FILE_REG,"\nEstado de Marco modificado deberia: %d\n",marco_a_reemplazar->MODIFICADA);

	if (marco_a_reemplazar->MODIFICADA == 1){
		//Significa que debo escribir la pagina modificada en SWAP
		Direccion* direccion_reemplazo = new_direccion_vacia();
		direccion_reemplazo->BUFFER = 0; //No envio buffer
		direccion_reemplazo->OFFSET = 0; //No importa el buffer es pagina completa
		direccion_reemplazo->PAG = marco_a_reemplazar->PAGINAID;
		direccion_reemplazo->PID = marco_a_reemplazar->PID;
		direccion_reemplazo->SIZE = memoria->MARCO_SIZE;

		char* contenido_pagina = memoria->VECTOR_MEMORIA[marco_a_reemplazar->MARCOID];

		escribir_pagina_swap(memoria,direccion_reemplazo,contenido_pagina);
	}

	Marco* marco_nuevo = new_marco(marco_a_reemplazar->MARCOID,direccion->PID,direccion->PAG,0);
	reemplazar_marcos(memoria->marcos,marco_a_reemplazar,marco_nuevo);

	memoria->VECTOR_MEMORIA[marco_nuevo->MARCOID] = leer_pagina_swap(memoria,direccion);

	//printf("\nmarcos reemplazados\n");
	//save_log(memoria->FILE_REG,"\nmarcos reemplazados\n");

	//crear_hilo_un_parametro(hilo_mostrar_estado_memoria,memoria);
}

char* leer_pagina_swap(Memoria* memoria,Direccion* direccion){
	pthread_mutex_lock(&memoria->sem_swap); //Ocupo el SWAP
	//printf("\nEnviar 31, solicitud de envio direccion a SWAP:%d \n",memoria->DESCRIPTOR_SWAP);
	//save_log(memoria->FILE_REG,"\nEnviar 31, solicitud de envio direccion a SWAP\n");
	int dato_enviar = 31;  //Enviar solicitud de direccion
	escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&dato_enviar,sizeof(int));

	int dato_leido;
	leer_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&dato_leido,sizeof(int));
	if (dato_leido == 32){ // El Swap respondio la solicitud de direccion
		//printf("\nSwap Listo para recibir Direccion: %d \n", memoria->DESCRIPTOR_SWAP);
		//save_log(memoria->FILE_REG,"\nSwap Listo para recibir Direccion\n");
		escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)direccion, sizeof *direccion);
		//printf("\nSe envio la direccion al SWAP\n");
		//save_log(memoria->FILE_REG,"\nSe envio la direccion al SWAP\n");
		char* cadena_instruccion = malloc(memoria->MARCO_SIZE);
		leer_descriptor(memoria->DESCRIPTOR_SWAP,(char*)cadena_instruccion,memoria->MARCO_SIZE);
		//printf("\n%s\n",cadena_instruccion);
		//save_log_char(memoria->FILE_REG,"\n%s\n",cadena_instruccion);
		pthread_mutex_unlock(&memoria->sem_swap); //Ocupo el SWAP
		return cadena_instruccion;
	}
	pthread_mutex_unlock(&memoria->sem_swap); //Ocupo el SWAP
	return "error";
}

void escribir_pagina_swap(Memoria* memoria,Direccion* direccion,char* Pagina){
	pthread_mutex_lock(&memoria->sem_swap); //Ocupo el SWAP
	//printf("\n63, solicitud de envio direccion a SWAP para escribir:%d \n",memoria->DESCRIPTOR_SWAP);
	//save_log(memoria->FILE_REG,"\n63, solicitud de envio direccion a SWAP para escribir:%d \n");
	int dato_enviar = 63;  //Enviar solicitud de direccion para almacenar
	escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&dato_enviar,sizeof(int));

	int dato_leido;
	leer_descriptor(memoria->DESCRIPTOR_SWAP,(char*)&dato_leido,sizeof(int));
	if (dato_leido == 64){ // El Swap respondio la solicitud de direccion para almacenar
		//printf("\n64 Swap Listo para recibir Direccion para almacenar: %d \n", memoria->DESCRIPTOR_SWAP);
		//save_log(memoria->FILE_REG,"\n64 Swap Listo para recibir Direccion para almacenar: \n");
		// 1 - Envio Direccion
		escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)direccion, sizeof *direccion);
		//printf("\nSe envio la direccion al SWAP\n");
		//save_log(memoria->FILE_REG,"\nSe envio la direccion al SWAP\n");
		// 2 - Envio Pagina
		escribir_descriptor(memoria->DESCRIPTOR_SWAP,(char*)Pagina, memoria->MARCO_SIZE);
	}
	pthread_mutex_unlock(&memoria->sem_swap); //Ocupo el SWAP
}
