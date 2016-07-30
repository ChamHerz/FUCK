#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include "consola.h"
#include "cliente.h"
#include "interprete.h"
#include "file.h"
//constructor

Consola* new_consola(){
	Consola* c;
	c = malloc(sizeof(struct consola));
	return c;
}

void set_path_archivo_configuracion(Consola* consola,char* path){
	consola->PATH = path;
}

void cargar_archivo_configuracion(Consola* consola){
	//if(string_equals_ignore_case(consola->PATH,"consola.conf")){
	//	consola->PATH = "../consola.conf";
	//}

	printf("\nfile config: %s\n",consola->PATH);

	t_config* config_file = config_create(consola->PATH);
	consola->IP_NUCLEO = config_get_string_value(config_file,"IP_NUCLEO");
	consola->PUERTO_NUCLEO = config_get_string_value(config_file,"PUERTO_NUCLEO");

	printf("Archivo de configuracion cargado \n");
}

void set_path_ansisop(Consola* consola, char* path_ansisop){
	consola->PATH_ANSISOP = path_ansisop;
}

void consola_inciar(Consola* consola){
//	consola->hilo_para_nucleo = crear_hilo_un_parametro(hilo_para_nucleo,consola);
	//Esperar a que terminen los hilos creados
//	pthread_join(consola->hilo_para_nucleo, NULL);

	printf("\nHilo para Nucleo\n");

	consola->DESCRIPTOR_SERVIDOR = abrir_conexion(consola->IP_NUCLEO,consola->PUERTO_NUCLEO);
	printf("conexion aceptada %d \n", consola->DESCRIPTOR_SERVIDOR);

	//Envio Ansisop
	int resultado = comando_ansisop(consola);
	if (resultado){//entro ansisop
		esperar_ordenes_nucleo(consola);
	}
}

void esperar_ordenes_nucleo(Consola* consola){
	int bucle = 1;
	while (bucle){
		bucle = ejecutar_orden(consola);
	}
}

int ejecutar_orden(Consola* consola){
	int orden;
	int bytes_leidos = leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&orden,sizeof(int));
	if (bytes_leidos == 0){
		//cerro consola
		return 0;
	}
	else{
		//Hacer algo
		if (orden == 65){
			printf("%d - Nucleo avisa sobre fin de programa\n",orden);
			finalizar_consola(consola);
			return 0;
		}
		//Falta de frames
		if (orden == 733){
			printf("%d - Finaliza proceso por falta de Frames\n",orden);
			finalizar_consola(consola);
			return 0;
		}
		//Aborto del CPU justo en ejecucion
		if (orden == 788){
			printf("%d - PCB perdido en ejecucion de CPU\n",orden);
			finalizar_consola(consola);
			return 0;
		}
		if (orden == 54){
			printf("%d - solicitud de imrimir valor recibida\n",orden);
			imprimir_valor(consola);
			return 1;
		}
		if (orden == 62){
			printf("%d - solicitud de imrimir texto recibida\n",orden);
			imprimir_texto(consola);
			return 1;
		}
		return 1;
	}
}

void imprimir_valor(Consola* consola){
	int pid;
	int valor;
	leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&pid,sizeof(int));
	leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&valor,sizeof(int));
	printf("Se recibio PID: %d Valor: %d\n",pid,valor);
}

void imprimir_texto(Consola* consola){
	int pid;
	int size_cadena;
	char* cadena;
	leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&pid,sizeof(int));
	leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&size_cadena,sizeof(int));
	//cadena = malloc(size_cadena);
	cadena = calloc(size_cadena,sizeof(char));
	leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)cadena,size_cadena);
	printf("Se recibio PID: %d Texto: %s\n",pid,cadena);
}

void finalizar_consola(Consola* consola){
	//
}

//Comando para enviar un programa ansisop
int comando_ansisop(Consola* consola){
	Archivo* archivo = new_archivo();
	archivo->path = consola->PATH_ANSISOP;
	archivo->modo = "r";  //lectura
	printf("\nLeer ansisop: %s\n",archivo->path);
	char* cadena_file = leer_archivo(archivo);
	int size_archivo = string_length(cadena_file);
	destroy_archivo(archivo);

	//Envio size

	int dato = 21; //Solicitud de Ansisop
	escribir_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&dato,sizeof(int));

	int respuesta;
	int dato_leido;
	leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&dato_leido,sizeof(int));
	if (dato_leido == 22){//Nucleo ordena que envie el ansisop
		escribir_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&size_archivo,sizeof(int));
		escribir_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)cadena_file,size_archivo);
		leer_descriptor(consola->DESCRIPTOR_SERVIDOR,(char*)&respuesta,sizeof(int));
		if(respuesta == 71){//El nucleo puede cargar normalmente
			printf("\nSe cargo el ansisop\n");
			return 1;
		}
		if(respuesta == 72){//El nucleo avisa que no entra en el swap
			printf("\nNo entra en swap ni compactando...\n");
			return 0;
		}
	}

	return 0;
}


pthread_t crear_hilo_un_parametro(void(*funcion_externa)(Consola*),Consola* consola){
	pthread_t thd1;

	pthread_create (&thd1, NULL, (void*)funcion_externa, (Consola*)consola);

	return thd1;
}
