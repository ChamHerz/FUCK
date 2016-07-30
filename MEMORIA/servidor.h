#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <stdio.h>			//por el printf
#include <stdlib.h>			//por el malloc
#include <string.h>			//por el memset
#include <netdb.h>			//Tine el tipo addrinfo
#include <pthread.h>		//por los hilos

#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PORT_CONSOLAS "5000"
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar


int abrir_socket(char* puerto);

int aceptar_conexion_cliente(int descriptor_servidor);

int abrir_conexion(char* ip_servidor,char* puerto);

int leer_descriptor(int descriptor, char* buffer, int buffer_size);

int escribir_descriptor(int descriptor, char* buffer, int buffer_size);

void iniciar_select(int descriptor_servidor,void(*nuevo_cliente)(int),int(*accion_cliente)(int),void(*cerro_cliente)(int));

void *atender_cliente(void *arg);

#endif /* SERVIDOR_H_ */
