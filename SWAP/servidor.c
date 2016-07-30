/*
 * servidor.c
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */
#include "servidor.h"

//Esta es la funcion que se ejecuta para cada cliente
void *atender_cliente(void *arg)
{
	int cl = (long)arg;
	char package[PACKAGESIZE];
	int status = 1;		// Estructura que manjea el status de los recieve.

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0){
		status = recv(cl, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
	}

	close(cl);
	return NULL;
}

int abrir_socket(char* puerto){
	int descriptor;

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;

	int rc;
	rc = getaddrinfo(NULL, puerto, &hints, &serverInfo);

	descriptor = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (descriptor == -1){
		return -1;
	}

	if(bind(descriptor,serverInfo->ai_addr, serverInfo->ai_addrlen)==-1){
		close(descriptor);
		return -1;
	}
	freeaddrinfo(serverInfo);

	if(listen(descriptor, BACKLOG)==-1){
		close(descriptor);
		return -1;
	}

	return descriptor;
}

int aceptar_conexion_cliente(int descriptor_servidor){
	int descriptor_cliente;
	struct sockaddr addr;
	socklen_t addrlen = sizeof(addr);

	descriptor_cliente = accept(descriptor_servidor, &addr, &addrlen);
	if(descriptor_cliente == -1)
	{
		return -1;;
	}

	return descriptor_cliente;
}

int leer_descriptor(int descriptor, char* buffer, int buffer_size){
	int leido = 0;
	int leer = 0;

	//Verificar parametros de entrada
	if ((descriptor == -1) || (buffer == NULL) || (buffer_size == -1)){
		return -1;
	}

	//mientras que no terminemos de leer todos los datos
	while (leido < buffer_size){
		leer = read(descriptor, buffer + leido, buffer_size - leido);
		if(leer > 0){
			//conseguimos leer datos
			leido = leido + leer;
		}
		else if (leer == 0) {
			//Cuidado! si devuelve 0 se cerro la conexion
			return leido;
		}
		else if (leer == -1){
			// se produjo un error;
			return -1;
		}
	}
}

int escribir_descriptor(int descriptor, char* buffer, int buffer_size){
	int escrito = 0;
	int escribir = 0;

	if((descriptor == -1) || (buffer == NULL) || (buffer_size == -1))
		return -1;

	while(escrito < buffer_size){
		escribir = write(descriptor,buffer + escrito,buffer_size-escrito);
		if (escribir > 0){
			escrito = escrito + escribir;
		}
		else if (escribir == 0){
			return escrito;
		}
		else{
			return -1;
		}
	}
}

void iniciar_select(int descriptor_servidor,void(*nuevo_cliente)(int),int(*accion_cliente)(int)){
	//COMIENZO DEL SELECT
	int descriptor_cliente;
	int resultado_select;
	int resultado_lectura;
	int fd_maximo;
	int descriptor_actual;
	fd_set read_set;
	fd_set temp_set;

	//limpio el read_set y sumo el servidor al read_set
	FD_ZERO(&read_set);
	FD_SET(descriptor_servidor,&read_set);

	fd_maximo = descriptor_servidor;

	while(1){
		memcpy(&temp_set,&read_set,sizeof(temp_set));
		resultado_select = select(fd_maximo + 1,&temp_set, NULL, NULL, NULL);

		if (resultado_select < 0){
			printf("Error en select: %d \n", resultado_select);
		}
		else if(resultado_select > 0){
			//Algo paso en el select!!!!
			if(FD_ISSET(descriptor_servidor,&temp_set)){
				//significa que es el descriptor del servidor el que paso algo
				//entonces acepto la llamada.
				descriptor_cliente = aceptar_conexion_cliente(descriptor_servidor);

				if (descriptor_cliente < 0){
					printf("Error al aceptar el cliente: %d \n", descriptor_cliente);
				}
				else{
					//agrego el descriptor del cliente a mi FD_SET
					FD_SET(descriptor_cliente,&read_set);
					//si el descriptor_cliente es mayor, ahora mi maximo es el descriptor cliente
					fd_maximo = (fd_maximo < descriptor_cliente)?descriptor_cliente:fd_maximo;
					//Agregar consola.
					nuevo_cliente(descriptor_cliente);
				}
				//borro el descriptor_servidor de mi temp_set para evitar problemas a continuacion.
				FD_CLR(descriptor_servidor,&temp_set);
			}

			//para los clientes:
			for(descriptor_actual = 0; descriptor_actual < fd_maximo + 1; descriptor_actual++){
				if(FD_ISSET(descriptor_actual,&temp_set)){
					resultado_lectura = accion_cliente(descriptor_actual);
					if(resultado_lectura == 0){
						//se cerro la conexion
						close(descriptor_actual);
						FD_CLR(descriptor_actual,&read_set);
					}
				}
			}
		}
	}
}
