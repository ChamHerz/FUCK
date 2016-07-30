#include "cliente.h"

int abrir_conexion(char* ip_servidor,char* puerto){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	int direccion;
	direccion = getaddrinfo(ip_servidor, puerto, &hints, &serverInfo);;
	if(direccion == -1)
		return -1;

	int descriptor;
	descriptor = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	if(descriptor == -1)
		return -1;
	if(connect(descriptor, serverInfo->ai_addr, serverInfo->ai_addrlen)==-1)
		return -1;

	freeaddrinfo(serverInfo);	// No lo necesitamos mas
	return descriptor;
}

int leer_descriptor(int descriptor,char* buffer,int buffer_size){
	int leido = 0;
	int leer = 0;

	if ((descriptor == -1) || (buffer == NULL) || (buffer_size==-1))
		return -1;

	while(leido < buffer_size){
		leer = read(descriptor, buffer + leido, buffer_size - leido);
		if (leer > 0){
			leido = leido + leer;
		}
		else if (leer == 0){
			return leido;
		}
		else if (leer == -1){
			return -1;
		}
	}
}

int escribir_descriptor(int descriptor, char* buffer, int buffer_size){
	int escrito = 0;
	int escribir = 0;

	if((descriptor == -1) || (buffer == NULL) || (buffer_size==-1))
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
