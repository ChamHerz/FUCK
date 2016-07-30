#include "primitivas.h"
#include "pcb.h"
#include "cpu.h"

//Las funciones primitivas se menten dentro de una gran funcion por acceden al cpu.
void parsear_linea(Cpu* cpu,char* cadena_instruccion){
	Pcb* pcb = cpu->pcb;

	t_puntero found_definirVariable(t_nombre_variable variable){
		printf("\nAnalizando Variable %c \n",variable);
		return agregar_definicion_variable(pcb,variable);
	}

	t_puntero found_obtenerPosicionVariable(t_nombre_variable variable) {
		printf("Obtener posicion de %c\n", variable);
		Address* address;
		address = malloc(sizeof(struct address));
		address = obtener_direccion_variable(pcb,variable);
		printf("Direccion encontrada %d %d %d\n",address->PAG,address->OFFSET,address->SIZE);
		return address->OFFSET;
	}

	void found_asignar(t_puntero puntero, t_valor_variable valor_variable) {
		printf("Asignando en %d el valor %d\n", puntero, valor_variable);
		Address* address;
		address = malloc(sizeof(struct address));
		address->OFFSET = puntero;
		asignar_valor_variable(cpu,address,valor_variable);
	}

	t_valor_variable found_dereferenciar(t_puntero puntero) {
		Address* address;
		address = malloc(sizeof(struct address));
		address->OFFSET = puntero;
		int contenido_variable;
		contenido_variable = dereferenciar_direccion(cpu,address);

		printf("Dereferenciar %d y su valor es: %d\n", puntero, contenido_variable);
		return contenido_variable;
	}

	void found_finalizar(){
		printf("Se finalizo el Programa\n");
		finalizar_programa(cpu,pcb);
	}

	void found_wait (t_nombre_semaforo identificador_semaforo){
		char semaforo = *identificador_semaforo;
		printf("Ejecutar Wait a \"%c\"\n",semaforo);

		enviar_wait(cpu,semaforo);
	}

	t_valor_variable found_obtenerValorGlobal(t_nombre_compartida variable){
		//arreglar variable
		//char* nombre_variable = variable;
		//nombre_variable[string_length(nombre_variable)] = '\0';

		char* variable_arreglada = string_new();
		string_append(&variable_arreglada,variable);
		string_trim(&variable_arreglada);

		int valor = obtener_valor_variable_global(cpu,variable_arreglada);
		return valor;
	}

	t_valor_variable found_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
		char* variable_global = variable;
		int variable_valor = valor;

		printf("Asignar a %s el valor de %d \n",variable_global,variable_valor);
		asignar_valor_variable_global(cpu,variable_global,valor);
		return variable_valor;
	}

	void found_signal(t_nombre_semaforo identificador_semaforo){
		char semaforo = *identificador_semaforo;
		printf("Ejecutar Signal a \"%c\"\n",semaforo);
		enviar_signal(cpu,semaforo);
	}

	void found_irAlLabel(t_nombre_etiqueta etiqueta){
		//arreglar etiqueta
		char* nombre_etiqueta = etiqueta;
		nombre_etiqueta[string_length(nombre_etiqueta)] = '\0';
		printf("Size de la etiqueta %d\n",string_length(nombre_etiqueta));
		printf("Ir a la etiqueta a %s\n",nombre_etiqueta);
		ejecutar_etiqueta(cpu,nombre_etiqueta);
	}

	void found_imprimir(t_valor_variable valor){
		int valor_variable = valor;
		imprimir(cpu,valor_variable);
	}

	void found_imprimirTexto(char* texto) {
		char* cadena_a_imprimir = texto;
		imprimirTexto(cpu,cadena_a_imprimir);
	}

	void found_entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
		printf("Se aplicara un retardo de %d por uso de %s",tiempo,dispositivo);
		char* variable_io = dispositivo;
		int time = tiempo;
		enviar_entrada_salida(cpu,variable_io,time);
	}

	void found_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
		char* label = etiqueta;
		int puntero_retorno = donde_retornar;
		ejecutar_llamadaConRetorno(cpu,label,puntero_retorno);
	}

	void found_retornar(t_valor_variable retorno) {
		int valor_retornado = retorno;
		ejecutar_retornar(cpu,valor_retornado);
	}

	AnSISOP_funciones functions = {
			.AnSISOP_definirVariable 		 = found_definirVariable,
			.AnSISOP_obtenerPosicionVariable = found_obtenerPosicionVariable,
			.AnSISOP_asignar				 = found_asignar,
			.AnSISOP_dereferenciar			 = found_dereferenciar,
			.AnSISOP_finalizar				 = found_finalizar,
			.AnSISOP_obtenerValorCompartida  = found_obtenerValorGlobal,
			.AnSISOP_asignarValorCompartida  = found_asignarValorCompartida,
			.AnSISOP_irAlLabel 				 = found_irAlLabel,
			.AnSISOP_imprimir 				 = found_imprimir,
			.AnSISOP_imprimirTexto 			 = found_imprimirTexto,
			.AnSISOP_entradaSalida 			 = found_entradaSalida,
			.AnSISOP_llamarConRetorno 		 = found_llamarConRetorno,
			.AnSISOP_retornar 				 = found_retornar,
	};
	AnSISOP_kernel kernel_functions = {
			.AnSISOP_wait 					 = found_wait,
			.AnSISOP_signal 				 = found_signal,
	};

	analizadorLinea(cadena_instruccion,&functions,&kernel_functions);

}

void ejecutar_retornar(Cpu* cpu,int retorno){
	Pcb* pcb = cpu->pcb;
	printf("\nretornar %d\n",retorno);

	Address* address_retorno = &cpu->pcb->STACK->RET_VAR[cpu->pcb->STACK->POS];
	//found_asignar(puntero_retorno,retorno);
	printf("\nDireccion de retorno\n");
	printf("OFFSET: %d\n",cpu->pcb->STACK->RET_VAR[cpu->pcb->STACK->POS].OFFSET);

	//Tengo que desapilar las variables cargadas.
	int index = 0;
	int cantidad_a_borrar = 0;
	while(index < pcb->STACK->VARS_SIZE){
		//Buscar solo las variables de la funcion actual
		if(pcb->STACK->VARS[index].POS == pcb->STACK->POS){
			cantidad_a_borrar++;
		}
		index++;
	}
	cpu->pcb->PC = cpu->pcb->STACK->RET_POS[cpu->pcb->STACK->POS];

	pcb->STACK->VARS_SIZE = pcb->STACK->VARS_SIZE - cantidad_a_borrar;

	pcb->STACK->POS--;

	asignar_valor_variable(cpu,address_retorno,retorno);
}

void ejecutar_llamadaConRetorno(Cpu* cpu,char* etiqueta,int puntero_retorno){
	printf("\nllamada de funcion %s y retornar en %d\n",etiqueta,puntero_retorno);
	Pcb* pcb = cpu->pcb;
	pcb->STACK->POS++;

	char* nombre_etiqueta_arreglado = _string_trim(etiqueta);
	t_puntero_instruccion retorno = metadata_buscar_etiqueta(nombre_etiqueta_arreglado,pcb->ETIQUETAS,pcb->ETIQUETAS_SIZE);
	printf("Se encuentra la etiqueta %s y corresponde ir a %d\n",nombre_etiqueta_arreglado,retorno);

	//almaceno en el stack el la posicion actual
	pcb->STACK->RET_POS = realloc(pcb->STACK->RET_POS,( pcb->STACK->POS + 1) * sizeof *pcb->STACK->RET_POS);
	pcb->STACK->RET_POS[pcb->STACK->POS] = pcb->PC;

	//almaceno lugar donde voy a retornar
	pcb->STACK->RET_VAR = realloc(pcb->STACK->RET_VAR,( pcb->STACK->POS + 1 ) * sizeof *pcb->STACK->RET_VAR);
	pcb->STACK->RET_VAR[pcb->STACK->POS].PAG = 0;
	pcb->STACK->RET_VAR[pcb->STACK->POS].OFFSET = puntero_retorno;
	pcb->STACK->RET_VAR[pcb->STACK->POS].SIZE = 4;

	printf("\nDireccion de retorno\n");
	printf("OFFSET: %d\n",pcb->STACK->RET_VAR[pcb->STACK->POS].OFFSET);

	pcb->PC = retorno - 1;
}

void enviar_entrada_salida(Cpu* cpu, char* dispositivo, int tiempo){
	avisar_nucleo_ejecucion_io(cpu,dispositivo,tiempo);
}

void imprimir(Cpu* cpu,int valor_variable){
	int valor = valor_variable;
	avisar_nucleo_imprimir_valor(cpu,valor);
}

void imprimirTexto(Cpu* cpu, char* texto) {
	char* cadena = texto;
	avisar_nucleo_imprimir_texto(cpu,cadena);
}

void ejecutar_etiqueta(Cpu* cpu,char* nombre_etiqueta){
	Pcb* pcb = cpu->pcb;
	printf("Etiquetas size: %d\n",pcb->ETIQUETAS_SIZE);
	printf("Etiquetas : %s\n",pcb->ETIQUETAS);
	char* nombre_etiqueta_arreglado = _string_trim(nombre_etiqueta);
	t_puntero_instruccion retorno = metadata_buscar_etiqueta(nombre_etiqueta_arreglado,pcb->ETIQUETAS,pcb->ETIQUETAS_SIZE);
	printf("Se encontra la etiqueta %s y corresponde ir a %d\n",nombre_etiqueta_arreglado,retorno);
	pcb->PC = retorno - 1;
}


int obtener_valor_variable_global(Cpu* cpu,char* variable_global){
	//printf("Obtener valor de variable Global %s\n",variable_global);
	//if(variable_global[string_length(variable_global)-1] == '\n'){
	//	variable_global[string_length(variable_global)-1] = '\0';
	//}
	int valor;
	valor = avisar_nucleo_variable_global(cpu,variable_global);
	return valor;
}

void asignar_valor_variable_global(Cpu* cpu,char* variable_global,int valor){
	char* variable = variable_global;
	avisar_nucleo_variable_global_modificada(cpu,variable,valor);
}

void enviar_wait(Cpu* cpu,char semaforo){
	avisar_nucleo_ejecucion_wait(cpu,semaforo);
}

void enviar_signal(Cpu* cpu,char semaforo){
	avisar_nucleo_ejecucion_signal(cpu,semaforo);
}

void finalizar_programa(Cpu* cpu, Pcb* pcb){
	//No necesito devolver el PCB porque lo voy a eliminar en el nucleo
	avisar_nucleo_final_programa(cpu,pcb);
}

int dereferenciar_direccion(Cpu* cpu,Address* address){
	Direccion* direccion;
	direccion = malloc(sizeof(struct direccion));

	int OFFSET = address->OFFSET;

	int sumar_pag = 0;

	if(OFFSET >= cpu->pcb->PAG_SIZE){
		//significa que la variable a buscar se encuentra en otra pag del stack
		sumar_pag = OFFSET / cpu->pcb->PAG_SIZE;
		OFFSET = OFFSET % cpu->pcb->PAG_SIZE;
	}
	if (sumar_pag >= cpu->pcb->PAG_STACK){
		cpu->pcb->ESTADO_FINAL = SEGMENTO_FAULT;
		cpu->pcb->QUANTUM_ACTUAL = cpu->pcb->QUANTUM;
		return -1;
	}
	else{
		direccion->PID = cpu->pcb->PID;
		//direccion->PAG = cpu->pcb->CANT_PAG_CODIGO + 1;
		direccion->PAG = cpu->pcb->CANT_PAG_CODIGO + sumar_pag; //comienza stack
		direccion->OFFSET = OFFSET;
		direccion->SIZE = 4;

		char* dato_leido;
		dato_leido = solicitar_a_memoria_direccion(cpu,direccion);

		int* new_buffer;
		new_buffer = malloc(sizeof(int));
		new_buffer = (int*)dato_leido;

		int dato_cacheado = *new_buffer;
		free(new_buffer);

		printf("\nValor obtenido desde memmoria: %d\n",dato_cacheado);

		return dato_cacheado;
	}
}

void asignar_valor_variable(Cpu* cpu,Address* address,t_valor_variable valor_variable){
	Direccion* direccion;
	direccion = malloc(sizeof(struct direccion));

	int OFFSET = address->OFFSET;

	int sumar_pag = 0;
	if(OFFSET >= cpu->pcb->PAG_SIZE){
		//significa que la variable a almacenar se encuentra en otra pag del stack
		sumar_pag = OFFSET / cpu->pcb->PAG_SIZE;
		OFFSET = OFFSET % cpu->pcb->PAG_SIZE;
	}
	if (sumar_pag >= cpu->pcb->PAG_STACK){
		cpu->pcb->ESTADO_FINAL = SEGMENTO_FAULT;
		cpu->pcb->QUANTUM_ACTUAL = cpu->pcb->QUANTUM;
	}
	else{
		direccion->PID = cpu->pcb->PID;
		//direccion->PAG = cpu->pcb->CANT_PAG_CODIGO + 1;
		direccion->PAG = cpu->pcb->CANT_PAG_CODIGO + sumar_pag; //comienzo del stack
		direccion->OFFSET = OFFSET;
		direccion->SIZE = 4;
		direccion->BUFFER = valor_variable;
		almacenar_bytes_en_pag(cpu,direccion);
	}
}

Address* obtener_direccion_variable(Pcb* pcb,t_nombre_variable variable){
	Address* address;
	printf("Se buscar la variable %c\n",variable);
	int index = 0;
	while(index <= pcb->STACK->VARS_SIZE){
		//Buscar solo las variables de la funcion actual
		if(pcb->STACK->VARS[index].POS == pcb->STACK->POS && pcb->STACK->VARS[index].ID == variable){
			address = &pcb->STACK[0].VARS[index];
			//printf("Direccion encontrada %d %d %d\n",address->PAG,address->OFFSET,address->SIZE);
			return address;
		}
		index++;
	}
	return NULL;
}

int agregar_definicion_variable(Pcb* pcb,t_nombre_variable variable){
	printf("PCB pos %d\n",pcb->STACK->POS);
	printf("================== \n");

	if (pcb->STACK->VARS_SIZE == 0){
		//Significa que nunca se agrego una variable
		//Agregar nueva variable al vector de variables
		printf("PCB vars_size %d\n",pcb->STACK->VARS_SIZE);
		printf("Size del VARS %d\n",sizeof *pcb->STACK->VARS);
		pcb->STACK->VARS = malloc(sizeof *pcb->STACK->VARS);
	}
	else{
		//Significa que ya tiene una variable agregada
		//Se incrementa el vector en una posicion
		pcb->STACK->VARS = realloc(pcb->STACK->VARS,( pcb->STACK->VARS_SIZE + 1 ) * sizeof *pcb->STACK->VARS);
	}

	int OFFSET = pcb->STACK[0].VARS_SIZE * 4;

	//int OFFSET = pcb->STACK[0].VARS_SIZE * 4;
	int sumar_pag = 0;
	if(OFFSET >= pcb->PAG_SIZE){
		//significa que la variable a almacenar se encuentra en otra pag del stack
		sumar_pag = OFFSET / pcb->PAG_SIZE;
		//OFFSET = OFFSET % pcb->PAG_SIZE;
	}
	if (sumar_pag >= pcb->PAG_STACK){
		pcb->ESTADO_FINAL = SEGMENTO_FAULT;
		pcb->QUANTUM_ACTUAL = pcb->QUANTUM;
	}
	//else{
		pcb->STACK->VARS[pcb->STACK->VARS_SIZE].POS = pcb->STACK->POS;
		pcb->STACK->VARS[pcb->STACK->VARS_SIZE].ID = variable;
		pcb->STACK->VARS[pcb->STACK->VARS_SIZE].PAG = 0;
		pcb->STACK->VARS[pcb->STACK->VARS_SIZE].OFFSET = pcb->STACK[0].VARS_SIZE * 4;
		pcb->STACK->VARS[pcb->STACK->VARS_SIZE].SIZE = 4;

		printf("Referencia agregada POS %d\n",pcb->STACK->VARS[pcb->STACK->VARS_SIZE].POS);
		printf("Referencia agregada id %c\n",pcb->STACK->VARS[pcb->STACK->VARS_SIZE].ID);
		printf("Referencia agregada pag %d\n",pcb->STACK->VARS[pcb->STACK->VARS_SIZE].PAG);
		printf("Referencia agregada offset %d\n",pcb->STACK->VARS[pcb->STACK->VARS_SIZE].OFFSET);
		printf("Referencia agregada size %d\n",pcb->STACK->VARS[pcb->STACK->VARS_SIZE].SIZE);

		int offset = pcb->STACK[0].VARS[pcb->STACK[0].VARS_SIZE].OFFSET;

		pcb->STACK[0].VARS_SIZE++;
	//}

	return offset;
}
