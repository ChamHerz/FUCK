#include <commons/config.h>
#include "cpu.h"
#include "cliente.h"
#include "direccion.h"

#include <signal.h>	//por el SIGUSR1

//constructor
Cpu* new_cpu(){
	Cpu* c;
	c = malloc(sizeof(struct cpu));

	//Propiedades
//	c->OCUPADO = 0;
//	c->SEMAFORO_BLOQUEO = 0;
//	c->evento_cierre_por_SISGUR1 = 0;
//	c->evento_finalizar_CPU = 0;
	return c;
}

void set_path_conf(Cpu* cpu, char* path){
	cpu->PATH = path;
}

void cargar_archivo_configuracion(Cpu* cpu){
	t_config* config_file = config_create(cpu->PATH);
	cpu->IP_NUCLEO = config_get_string_value(config_file,"IP_NUCLEO");
	cpu->PUERTO_NUCLEO = config_get_string_value(config_file,"PUERTO_NUCLEO");
	cpu->IP_MEMORIA = config_get_string_value(config_file,"IP_MEMORIA");
	cpu->PUERTO_MEMORIA = config_get_string_value(config_file,"PUERTO_MEMORIA");

	//cargo el id processo
	cpu->IP_PROCESS = process_getpid();

	printf("\nArchivo de configuracion cargado \n");
}

void crear_hilo_un_para_sisgur(void(*funcion_externa)(Cpu*),Cpu* cpu){
	pthread_attr_t atributos;
	pthread_attr_init(&atributos);
	pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_DETACHED); //Liberar recursos automaticamente
	pthread_t thd1;
	pthread_create (&thd1, &atributos, (void*)funcion_externa, (Cpu*)cpu);
}


void iniciar_escuchar_sigusr1(Cpu* cpu){
	void tratarSenalSIGUSR1(int numSenal){
		printf ("Recibida señal del cierre seguro\n");
		cpu->evento_cierre_por_SISGUR1 = 1;
	}

	while (1){
		signal (SIGUSR1, tratarSenalSIGUSR1);
	}
}


void cpu_inciar(Cpu* cpu){
	cpu->hilo_para_nucleo = crear_hilo_un_parametro(hilo_para_nucleo,cpu);
	//cpu->hilo_para_sigusr1 = crear_hilo_un_parametro(iniciar_escuchar_sigusr1,cpu);

	//esperar a que termine
	pthread_join(cpu->hilo_para_nucleo, NULL);
	//pthread_join(cpu->hilo_para_sigusr1, NULL);
}

pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Cpu*),Cpu* cpu){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (Cpu*)cpu);

	return thd1;
}

void hilo_para_nucleo(Cpu* cpu){

	printf("\nConexion con Memoria\n");

	cpu->DESCRIPTOR_MEMORIA = abrir_conexion(cpu->IP_MEMORIA,cpu->PUERTO_MEMORIA);
	printf("conexion aceptada por memoria %d \n", cpu->DESCRIPTOR_MEMORIA);

	printf("\nConexion con Nucleo\n");

	cpu->DESCRIPTOR_NUCLEO = abrir_conexion(cpu->IP_NUCLEO,cpu->PUERTO_NUCLEO);
	printf("conexion aceptada por nucleo %d \n", cpu->DESCRIPTOR_NUCLEO);

	//Uso un hilo para el Sisgur
	//crear_hilo_un_para_sisgur(iniciar_escuchar_sigusr1,cpu);

	int bucle = 1;
	while(bucle){
		bucle = esperar_orden_nucleo(cpu);
	}
}

int esperar_orden_nucleo(Cpu* cpu){
	int dato_leido;
	int dato_enviar;
	printf("\nESPERANDO PCB DEL NUCLEO...\n");
	int bytes_leidos = leer_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&dato_leido,sizeof(int));
	if (bytes_leidos <= 0){
		//cerro cpu
		return 0;
	}
	else{
		//Hacer algo
		if (dato_leido==27){//Solicitud del Nucleo, El nucleo piensa enviar un PCB
			printf("\n%d - Solicitud recibida del Nucleo para Enviar PCB\n",dato_leido);
			dato_enviar = 28;//Aviso al Nucleo que el CPU esta listo para recibir el PCB
			printf("\n%d - Avisar al nucleo que envie PCB\n",dato_enviar);
			escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
			cpu->pcb = recepcion_pcb(cpu);
			ejecutar_quantum(cpu);
			//cpu->OCUPADO = 0;
		}
		if(dato_leido==200){//Solicitud del nucleo, para que te destruyas
			close(cpu->DESCRIPTOR_NUCLEO);
			close(cpu->DESCRIPTOR_MEMORIA);
			return 0;
		}
		return 1;
	}
}

Pcb* recepcion_pcb(Cpu* cpu){
	printf("\nAqui voy recibir el PCB\n");
	Pcb* pcb = new_pcb();
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)pcb,sizeof *pcb);
	printf("Se recibira estructura INSTRUCCIONES de %d y de %d\n",pcb->INSTRUCCIONES_SIZE,sizeof *pcb->INSTRUCCIONES);
	pcb->INSTRUCCIONES = malloc(pcb->INSTRUCCIONES_SIZE * sizeof *pcb->INSTRUCCIONES);
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)pcb->INSTRUCCIONES,pcb->INSTRUCCIONES_SIZE * sizeof *pcb->INSTRUCCIONES);
	printf("\nMalloc del STACK %d\n",sizeof *pcb->STACK);
	pcb->STACK = malloc(sizeof *pcb->STACK);
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)pcb->STACK,sizeof *pcb->STACK);
	printf("Se recibira estructura ETIQUETAS de %d\n",pcb->ETIQUETAS_SIZE);
	pcb->ETIQUETAS = malloc(pcb->ETIQUETAS_SIZE);
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)pcb->ETIQUETAS,pcb->ETIQUETAS_SIZE);
	printf("Se recibira estructura VARS de %d y de %d",pcb->STACK->VARS_SIZE,sizeof *pcb->STACK->VARS);
	pcb->STACK->VARS = calloc(pcb->STACK->VARS_SIZE,sizeof *pcb->STACK->VARS);
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)pcb->STACK->VARS,pcb->STACK->VARS_SIZE * sizeof *pcb->STACK->VARS);
	pcb->STACK->RET_VAR = malloc((pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_VAR);
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)pcb->STACK->RET_VAR,(pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_VAR);
	pcb->STACK->RET_POS = malloc((pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_POS);
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)pcb->STACK->RET_POS,(pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_POS);
	printf("\nSTACK->POS...%d\n",pcb->STACK->POS);

	printf("intrucciones_size %d\n",pcb->INSTRUCCIONES_SIZE);
	printf("intrucciones_cant %d\n",pcb->INSTRUCCIONES_CANT);
	printf("intrucciones_start %d\n",pcb->INSTRUCCION_START);
	printf("Program_counter %d\n",pcb->PC);
	printf("Primera instruccion %d\n",pcb->INSTRUCCIONES[0].start);
	printf("Offset %d\n",pcb->INSTRUCCIONES[0].offset);

	//Almaceno el puntero del PCB en la variable del CPU
	cpu->PAG_SIZE = pcb->PAG_SIZE;
	return pcb;
}

void ejecutar_quantum(Cpu* cpu){
	Pcb* pcb = cpu->pcb;
	//Ejecutar el Quantum
	pcb->ESTADO_FINAL = QUANTUM_COMPLETO; //Valor por Default
	while(pcb->QUANTUM_ACTUAL <= pcb->QUANTUM &&
			pcb->ESTADO_FINAL != BLOQUEO_WAIT &&
			pcb->ESTADO_FINAL != BLOQUEO_IO){
		//incremento el PC
		pcb->PC++;
		int PC = pcb->PC;
		//Solicito a la memoria las paginas necesarias
		Direccion* direccion = new_direccion(pcb->INSTRUCCIONES[PC].start,pcb->INSTRUCCIONES[PC].offset,pcb->PAG_SIZE);
		direccion->PID = pcb->PID;
		printf("Pag: %d\n",direccion->PAG);
		printf("offset: %d\n",direccion->OFFSET);
		printf("size: %d\n",direccion->SIZE);

		int total_size = direccion->SIZE;
		int doble_cadena = 0;
		Direccion* direccion_add;
		if (direccion->OFFSET + direccion->SIZE > pcb->PAG_SIZE){//significa doble pagina
			//Significa que la cadena de instruccion estan justo entre 2 paginas
			//necesito crear una nueva solicitud
			direccion_add = new_direccion_vacia();
			int size_restante = direccion->OFFSET + direccion->SIZE - pcb->PAG_SIZE;
			direccion_add->PID = direccion->PID;
			direccion_add->PAG = direccion->PAG + 1; //Pagina siguiente
			direccion_add->SIZE = size_restante;
			direccion_add->OFFSET = 0;
			doble_cadena = 1;
			printf("\nla solicitud supera la pag. se requiere pag siguiente\n");
			//save_log(memoria->FILE_REG,"\nla solicitud supera la pag. se requiere pag siguiente\n");
			printf("PID: %d\n",direccion_add->PID);
			printf("PAG: %d\n",direccion_add->PAG);
			printf("OFFSET %d\n",direccion_add->OFFSET);
			printf("SIZE %d\n",direccion_add->SIZE);
			//save_log_int(memoria->FILE_REG,"PID: %d\n",direccion_add->PID);
			//save_log_int(memoria->FILE_REG,"PAG: %d\n",direccion_add->PAG);
			//save_log_int(memoria->FILE_REG,"OFFSET %d\n",direccion_add->OFFSET);
			//save_log_int(memoria->FILE_REG,"SIZE %d\n",direccion_add->SIZE);
			//save_log_int(memoria->FILE_REG,"BUFFER %d\n",direccion_add->BUFFER);

			//no puedo leer el total de bytes de la primera pagina, hago overflow
			//actualizo el size
			direccion->SIZE = pcb->PAG_SIZE - direccion->OFFSET;
		}

		char* cadena_instruccion;
		cadena_instruccion = malloc(direccion->SIZE);
		cadena_instruccion = solicitar_a_memoria_direccion(cpu,direccion);

		char* cadena_total;
		if(doble_cadena){//significa que es doble pagina
			printf("Lectura de pag siguiente\n");
			//save_log(memoria->FILE_REG,"Lectura de pag siguiente\n");
			printf("PID: %d\n",direccion_add->PID);
			printf("PAG: %d\n",direccion_add->PAG);
			printf("OFFSET %d\n",direccion_add->OFFSET);
			printf("SIZE %d\n",direccion_add->SIZE);
			//save_log_int(memoria->FILE_REG,"PID: %d\n",direccion_add->PID);
			//save_log_int(memoria->FILE_REG,"PAG: %d\n",direccion_add->PAG);
			//save_log_int(memoria->FILE_REG,"OFFSET %d\n",direccion_add->OFFSET);
			//save_log_int(memoria->FILE_REG,"SIZE %d\n",direccion_add->SIZE);
			//save_log_int(memoria->FILE_REG,"BUFFER %d\n",direccion_add->BUFFER);

			char* cadena_dos;
			cadena_dos = malloc(direccion_add->SIZE);
			cadena_dos = solicitar_a_memoria_direccion(cpu,direccion_add);

			cadena_total = string_new();
			string_append(&cadena_total,cadena_instruccion); //primera pag.
			string_append(&cadena_total,cadena_dos); //resto de la segunda.
		}
		else{
			cadena_total = cadena_instruccion;
		}

		int size_cadena = string_length(cadena_total);
		int index = 0;
		int cadena_cortada = 0;
		while (cadena_cortada == 0 & index <= size_cadena){
			if (cadena_total[index] == '\n'){
				cadena_cortada = 1;
				cadena_total[index] = '\0';
			}
			index++;
		}
		char* cadena_arreglada = string_new();
		cadena_arreglada = string_substring(cadena_total,0,index);

		printf("\n%s\n",cadena_arreglada);

		parsear_linea(cpu,cadena_arreglada);

		printf("\nQuantum %d de %d Finalizado",pcb->QUANTUM_ACTUAL,pcb->QUANTUM);
		printf("\nQuantum Sleep: %d milisegundos\n",pcb->QUANTUM_SLEEP / 1000);

		usleep(pcb->QUANTUM_SLEEP);

		pcb->QUANTUM_ACTUAL++;
	}


	//if(cpu->evento_cierre_por_SISGUR1 == 1){
		//Guardo en el PCB un Flag que indica que lo salve



		//Se tiene que cerrar el CPU
		//int dato_enviar = 66;
		//escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&dato_enviar,sizeof(int));
		//printf("\n%d Se notifica la CPU que se va cerrar por SISGUR1\n",dato_enviar);
		//int dato_recibido;
		//leer_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&dato_recibido,sizeof(int));
		//if(dato_recibido == 67){//Significa que el nucleo ya salvo el PID
			//printf("\n%d Nucleo avisa que ya se puede ir SISGUR1\n",dato_recibido);
			//printf("Se cierra CPU por SISGUR!\n");
			//cpu->evento_finalizar_CPU = 1;
	//	}
	//}
	if (cpu->evento_cierre_por_SISGUR1 == 1){
		//Significa que se habia mandado la senal de SISGUR1
		cpu->pcb->FLAG_SISGUR1 = 1; //Marco.. antes de enviar
	}

	if(pcb->ESTADO_FINAL == BLOQUEO_WAIT){
		printf("Desalojar PCB por bloqueo de wait \n");
		enviar_pcb_al_nucleo(cpu);
		destroy_pcb(cpu->pcb);
	}
	if(pcb->ESTADO_FINAL == BLOQUEO_IO){
		printf("Desalojar PCB por bloqueo de IO \n");
		enviar_pcb_al_nucleo(cpu);
		destroy_pcb(cpu->pcb);
	}
	if(pcb->ESTADO_FINAL == SEGMENTO_FAULT){
		printf("\NFINALIZADO POR STACK OVERFLOW\n");
		enviar_pcb_al_nucleo(cpu);
		destroy_pcb(cpu->pcb);
	}
	if(pcb->ESTADO_FINAL == FINAL_PROGRAMA){
		printf("\NPrograma Finalizado Normalmente \n");
		enviar_pcb_al_nucleo(cpu);
		destroy_pcb(cpu->pcb);
	}
	if(pcb->ESTADO_FINAL == QUANTUM_COMPLETO){
		pcb->QUANTUM_ACTUAL = 1;	//porque termino rafaja
		printf("Todos los Quantum terminaron, CPU Osiosa \n");
		printf("Se termino el quantum -> Desalojar PCB \n");
		enviar_pcb_al_nucleo(cpu);
		destroy_pcb(cpu->pcb);
	}
	if (cpu->evento_cierre_por_SISGUR1 == 1){
		printf("\nULTIMA RAFAGA KAMIKAZE, AUTO-DESTRUCTION recibida por algun MORTAL\n");
		pcb->QUANTUM_ACTUAL = 1;	//porque termino rafaja
		cpu->evento_finalizar_CPU = 1; //esta senal mata al CPU
	}
}

char* solicitar_a_memoria_direccion(Cpu* cpu,Direccion* direccion){
	int dato_enviar = 29; // Solicitud de envio de Direccion a Memoria
	escribir_descriptor(cpu->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
	printf("\n%d - Solicitud de direccion\n",dato_enviar);
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_MEMORIA,(char*)&dato_leido,sizeof(int));
	if (dato_leido==30){//Respuesta de la memoria, esta lista para recibir la Direccion
		printf("\n%d - Memoria lista para recepcion de direccion \n",dato_leido);
		escribir_descriptor(cpu->DESCRIPTOR_MEMORIA,(char*)direccion,sizeof *direccion);

		//char* cadena_instruccion = malloc(direccion->SIZE);
		char* cadena_instruccion = calloc(direccion->SIZE,sizeof(char));
		printf("\nEl CPU recibira %d Bytes desde Memoria \n",direccion->SIZE);
		leer_descriptor(cpu->DESCRIPTOR_MEMORIA,(char*)cadena_instruccion,direccion->SIZE);
		return cadena_instruccion;
	}
	return "Bien";
}

void avisar_nucleo_ejecucion_io(Cpu* cpu,char* identificador,int tiempo){
	printf("\n58 - Se envia a Nucleo IO %s de tiempo %d\n",identificador,tiempo);
	int dato_enviar = 58;
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&dato_enviar,sizeof(int));
	int leer_dato;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&leer_dato,sizeof(int));
	if(leer_dato == 59){//El Nucleo esta listo para IO
		char* io_id = identificador;
		int size_identificador = string_length(io_id);
		int time = tiempo;
		int PID = cpu->pcb->PID;
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&PID,sizeof(int));
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&size_identificador,sizeof(int));
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)io_id,size_identificador);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&time,sizeof(int));
		printf("Se envio a nucleo %s de size %d de tiempo %d",io_id,size_identificador,time);

		//Adelanto el quantum para que no siga parseando
		cpu->pcb->ESTADO_FINAL = BLOQUEO_IO;
		//cpu->QUANTUM_ACTUAL = cpu->pcb->QUANTUM;
	}
}

void enviar_pcb_al_nucleo(Cpu* cpu){
	int dato_enviar = 43; //Solicitud de Envio de PCB al Nucleo
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
	printf("\n%d - Solicitud al Nucleo para Enviar PCB\n",dato_enviar);
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_leido,sizeof(int));
	printf("\n%d - el Nucleo respondio que esta listo para recibir PCB\n",dato_leido);
	if(dato_leido == 44){//El nucleo esta listo para recibir el PCB
		//Se envia el PCB
		Pcb* pcb = cpu->pcb;
		printf("Envio el PCB...\n");

		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)pcb,sizeof *pcb);
		printf("Se enviara INSTRUCCIONES de %d y de %d\n",pcb->INSTRUCCIONES_SIZE,sizeof *pcb->INSTRUCCIONES);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)pcb->INSTRUCCIONES,pcb->INSTRUCCIONES_SIZE * sizeof *pcb->INSTRUCCIONES);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)pcb->STACK,sizeof *pcb->STACK);
		printf("Se envia estructura ETIQUETAS de %d\n",pcb->ETIQUETAS_SIZE);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)pcb->ETIQUETAS,pcb->ETIQUETAS_SIZE);
		printf("Se enviara VARS de %d y de %d\n",pcb->STACK->VARS_SIZE,sizeof *pcb->STACK->VARS);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)pcb->STACK->VARS,pcb->STACK->VARS_SIZE * sizeof *pcb->STACK->VARS);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)pcb->STACK->RET_VAR,(pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_VAR);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)pcb->STACK->RET_POS,(pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_POS);
		printf("Se envio PCB que ocupa...%d\n",sizeof *pcb);
		printf("El Motivo de desalojo es: %d\n",pcb->ESTADO_FINAL);
	}
}

void avisar_nucleo_imprimir_valor(Cpu* cpu,int valor_variable){
	printf("Se imprimira el valor %d en la consola\n",valor_variable);
	int dato_enviar = 52;
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
	printf("%d - Solicitud de Impresion de Valor\n",dato_enviar);
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_leido,sizeof(int));
	if (dato_leido==53){
		printf("%d - Respuesta de Impresion de Valor\n",dato_leido);
		int valor = valor_variable;
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&valor,sizeof(int));
		int pid = cpu->pcb->PID;
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&pid,sizeof(int));
	}
}

void avisar_nucleo_imprimir_texto(Cpu* cpu,char* cadena){
	printf("Se imprimira el texto %s en la consola\n",cadena);
	int dato_enviar = 60;
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
	printf("%d - Solicitud de Impresion de Texto\n",dato_enviar);
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_leido,sizeof(int));
	if (dato_leido==61){
		printf("%d - Respuesta de Impresion de Texto\n",dato_leido);
		int size_cadena = string_length(cadena);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&size_cadena,sizeof(int));
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)cadena,size_cadena);
		int pid = cpu->pcb->PID;
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&pid,sizeof(int));
	}
}

int avisar_nucleo_variable_global(Cpu* cpu,char* variable_global){
	printf("Se envia al Nucleo Variable Global %s\n",variable_global);
	int dato_enviar = 45;
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
	printf("%d - Solicitud de Valor de Variable Global\n",dato_enviar);
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_leido,sizeof(int));
	printf("%d - Dato leido desde nucleo, deberia darme 46\n",dato_leido);
	if (dato_leido==46){
		printf("%d - Respuesta de Valor de Variable Global\n",dato_leido);
		int size_variable_global = string_length(variable_global);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&size_variable_global,sizeof(int));
		//char* variable_global_tmp = variable_global;
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)variable_global,size_variable_global);
		printf("Se envio la variable global %s de size %d\n",variable_global,size_variable_global);
		int valor_variable;
		leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&valor_variable,sizeof(int));
		printf("Se recibio el valor de %d\n",valor_variable);
		return valor_variable;
	}
	return 0;
}

void avisar_nucleo_variable_global_modificada(Cpu* cpu,char* variable_global,int valor){
	printf("Se envia al Nucleo Variable Global a modificar %s\n",variable_global);
	int dato_enviar = 50;
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
	printf("%d - Solicitud de Modificar Variable Global\n",dato_enviar);
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_leido,sizeof(int));
	if (dato_leido==51){
		printf("%d - Respuesta de Modificar Variable Global\n",dato_leido);
		int size_variable_global = string_length(variable_global);
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&size_variable_global,sizeof(int));
		char* enviar_id_variable = variable_global;
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)enviar_id_variable,size_variable_global);
		printf("Se envio la variable global %s de size %d\n",enviar_id_variable,size_variable_global);
		int valor_variable = valor;
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&valor_variable,sizeof(int));
		printf("Se envio el valor de %d\n",valor_variable);
	}
}

void avisar_nucleo_ejecucion_signal(Cpu* cpu,char semaforo){
	printf("\n47 - Envio ejecucion de signal semaforo %c\n",semaforo);
	int dato_enviar = 47;
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_leido,sizeof(int));
	if (dato_leido==48){//Respuesta del Nucleo, listo para recibir id_semaforo
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&semaforo,sizeof(char));
		printf("\nSe envio al Nucleo el Id - Semaforo: %c\n",semaforo);
		int resultado_wait;
		leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&resultado_wait,sizeof(int));
		if (resultado_wait == 49){
			printf("NO Se bloqueo el Proceso en un Signal\n");
		}
	}
}

void avisar_nucleo_ejecucion_wait(Cpu* cpu,char semaforo){
	printf("\n39 avisar al Nucleo sobre la ejecucion de wait semaforo %c\n",semaforo);
	int dato_enviar = 39;
	escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_enviar,sizeof(int));
	int dato_leido;
	leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&dato_leido,sizeof(int));
	if (dato_leido==40){//Respuesta del Nucleo, listo para recibir id_semaforo
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&cpu->pcb->PID,sizeof(int));
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&semaforo,sizeof(char));
		printf("\nSe envio al Nucleo el Id - Semaforo: %c pid: %d\n",semaforo,cpu->pcb->PID);
		int resultado_wait;
		leer_descriptor(cpu->DESCRIPTOR_NUCLEO ,(char*)&resultado_wait,sizeof(int));
		if (resultado_wait == 41){
			printf("Se bloqueo el Proceso\n");

			//Marco el PCB Como bloqueo del Whait.
			//Se va para bloqueados. el quantum permanece
			cpu->pcb->ESTADO_FINAL = BLOQUEO_WAIT;
			cpu->pcb->SEMAFORO = semaforo;
			//cpu->QUANTUM_ACTUAL = cpu->pcb->QUANTUM;
		}
		if (resultado_wait == 42){
			printf("No Se bloqueo el Proceso\n");
		}
	}
}

void avisar_nucleo_final_programa(Cpu* cpu,Pcb* pcb){
	//Adelanto el quantum para que no siga parseando
	pcb->ESTADO_FINAL = FINAL_PROGRAMA;
	cpu->pcb->QUANTUM_ACTUAL = cpu->pcb->QUANTUM;
}

void almacenar_bytes_en_pag(Cpu* cpu,Direccion* direccion){
	printf("\n%d - Offset\n",direccion->OFFSET);
	printf("\n%d - Buffer\n",direccion->BUFFER);
	int dato_enviar = 35;
	escribir_descriptor(cpu->DESCRIPTOR_MEMORIA,(char*)&dato_enviar,sizeof(int));
	printf("\n%d - Solicitud de direccion para almacenar\n",dato_enviar);
	int dato_leido;
		leer_descriptor(cpu->DESCRIPTOR_MEMORIA,(char*)&dato_leido,sizeof(int));
		if (dato_leido==36){//Respuesta de la memoria, esta lista para recibir la Direccion para almacenar
			printf("\n%d - Memoria lista para recepcion de direccion para almacenar \n",dato_leido);
			escribir_descriptor(cpu->DESCRIPTOR_MEMORIA,(char*)direccion,sizeof *direccion);
		}
}
