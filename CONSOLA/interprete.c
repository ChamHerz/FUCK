#include "interprete.h"
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

//constructor
Interprete* new_interprete(){
	Interprete* i;
	i = malloc(sizeof(struct interprete));
	i->comandos = dictionary_create();

	dictionary_put(i->comandos,"fin","comando_fin");
	dictionary_put(i->comandos,"clear","comando_clear");
	dictionary_put(i->comandos,"reload-conf","comando_reload_conf");
	dictionary_put(i->comandos,"ping","comando_ping");
	dictionary_put(i->comandos,"send","comando_send");
	dictionary_put(i->comandos,"send-cpu","comando_send_cpu");
	dictionary_put(i->comandos,"ansisop","comando_ansisop");
	return i;
}

//Destructor
void destroy_interprete(Interprete* this){
	free(this);
}


int leer_cadena(char* cad,int n){
	int c,i;

	//Vacio el buffer
	for(i=1; i < n;i++)
		cad[i] = '\0';

	c=getchar();
	if(c == EOF){
		cad[0] = '\0';
		return 0;
	}

	//Apreto Enter de una
	if (c == '\n'){
		fflush(stdin);
		return 0;
	}
	else{
		cad[0] = c;
		i = 1;
	}

	for (;i < n-1 && (c=getchar())!=EOF && c!='\n';i++)
		cad[i] = c;

	if (c != '\n' && c != EOF)
		while ((c = getchar()) != '\n' && c != EOF);

	return 1;
}

int iniciar_interprete(Interprete* interprete){
	char cadena[MAX_CADENA];
	int resultado;
	int bucle = 1;

	while(bucle){
		printf(PROMPT);
		fflush(stdin);
		resultado = leer_cadena(cadena,MAX_CADENA);
		if (resultado != 0) { //Sino apreta enter de una
			//Ahora controlar cadena revisa el primer comando, si es invalido retorna 1
			//y sigue en el bucle, pero sino modifica el bucle saliendo
			bucle = controlar_cadena(cadena,interprete);
		}
	}

	return 1;
}

int controlar_cadena(char* cadena,Interprete* interprete){
	char* cadena_corregida = string_new();
	char** comandos;
	int resultado_busqueda_comando;

	string_append(&cadena_corregida,cadena);
	string_trim_left(&cadena_corregida);

	//obtener el primer comando
	comandos = string_split(cadena_corregida," ");

	resultado_busqueda_comando = buscar_comando(comandos[0],interprete);
	if(resultado_busqueda_comando == -1){
		printf("Comando invalido \n");
		interprete->comando_correcto = 0;
		return 1;
	}
	else{
		//Comando encontrado -> ir a funcion especifica
		interprete->comando_correcto = 1;
		interprete->commandos_ingresados = comandos;
		return 0;
	}
}

int buscar_comando(char* comando,Interprete* interprete){
	int resultado = -1;

	if(dictionary_has_key(interprete->comandos,comando))
		resultado = 1;

	return resultado;
}
