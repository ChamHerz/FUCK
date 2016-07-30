/*
 * cliente.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdio.h>			//por el printf
#include <stdlib.h>			//por el malloc
#include <string.h>			//por el memset
#include <netdb.h>			//Tine el tipo addrinfo
#include <commons/string.h>	//para concatenar el nick

#define PUERTO_CONSOLA "5000"
#define IP_SERVIDOR "10.16.19.51"
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

int abrir_conexion(char* ip_servidor,char* puerto);

int escribir_descriptor(int descriptor, char* buffer, int buffer_size);

int leer_descriptor(int descriptor,char* buffer,int buffer_size);

#endif /* CLIENTE_H_ */
