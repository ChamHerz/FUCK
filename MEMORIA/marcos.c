#include "marcos.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

//constructor
Marcos* new_marcos(){
	Marcos* m;
	m = malloc(sizeof(struct marcos));
	m->lista = list_create();
	m->MARCOID_ACTUAL = -1;

	return m;
}

//Destructor
void destroy_marcos(Marcos* marcos){
	list_destroy(marcos->lista);
	free(marcos);
}

int conseguir_id_marco_libre(Marcos* marcos){
	int MarcoID = 0;

	bool condicion_sort(Marco* marco1,Marco* marco2){
		return marco1->MARCOID < marco2->MARCOID;
	}

	bool condicion_busqueda(Marco* marco){
		if (marco->MARCOID == MarcoID){
			MarcoID++;
			return false;
		}
		else{
			return true;
		}
	}

	if (list_size(marcos->lista)==0){
		//Si esta vacia devuelvo marco 0
		return MarcoID;
	}
	else{
		//Ordeno por MarcoID
		list_sort(marcos->lista,(void*)condicion_sort);

		//Recorro la lista hasta que encuentre mi MarcoID Libre
		Marco* Marco_aux = list_find(marcos->lista,(void*)condicion_busqueda);
		return MarcoID;
	}
}

void pasar_todas_modificado(Marcos* marcos){
	void funcion_closure(Marco* marco){
		marco->MODIFICADA = 1;
	}

	list_iterate(marcos->lista,(void*)funcion_closure);
}

void agregar_marco(Marcos* marcos, Marco* marco){
	int MARCOID = conseguir_id_marco_libre(marcos);
	marco->MARCOID = MARCOID;
	list_add(marcos->lista,marco);
}

int cantidad_marcos_ocupados(Marcos* marcos){
	return list_size(marcos->lista);
}

Marco* buscar_marco(Marcos* marcos, int PID, int pagina){
	bool condicion_busqueda(Marco* marco){
		return ((marco->PID == PID) & (marco->PAGINAID == pagina));
	}

	return list_find(marcos->lista,(void*)condicion_busqueda);
}

Marco* buscar_marco_idMarco(Marcos* marcos, int MarcoID){
	bool condicion_busqueda(Marco* marco){
		return marco->MARCOID == MarcoID;
	}

	return list_find(marcos->lista,(void*)condicion_busqueda);
}

void set_bit_usado_marco(Marcos* marcos, int id_marco){
	Marco* marco = buscar_marco_idMarco(marcos,id_marco);
	marco->USADA = 1;
}

Marcos* marcos_un_proceso(Marcos* marcos,int PID){
	bool condicion_filtrado(Marco* marco){
		return marco->PID == PID;
	}
	Marcos* nueva_lista = new_marcos();
	nueva_lista->lista = list_filter(marcos->lista,(void*)condicion_filtrado);
	return nueva_lista;
}

Marco* buscar_marco_reemplaza_clock(Marcos* marcos,int PUNTERO_CLOCK){

	//Primera Psada, si esta usada la pasa a 0
	Marco* marco_a_reemplazar = buscar_marco_reemplazo_cambiar_usados(marcos,PUNTERO_CLOCK);
	if (marco_a_reemplazar == NULL){
		//No encontro en la primera pasada.. paso de nuevo, si o si.. encuentra
		marco_a_reemplazar = buscar_marco_reemplazo_cambiar_usados(marcos,PUNTERO_CLOCK);
	}

	return marco_a_reemplazar;
}

Marco* buscar_marco_reemplaza_clock_modificado(Marcos* marcos,int PID,int PUNTERO_CLOCK){

	//Primera Pasada.
	Marco* marco_a_reemplazar;
	marco_a_reemplazar = solo_buscar_usado_modificado(marcos,PUNTERO_CLOCK);//Primera Pasada
	if (marco_a_reemplazar == NULL){
		marco_a_reemplazar = buscar_usado_modificado_cambia(marcos,PUNTERO_CLOCK);//Segunda pasada
		if (marco_a_reemplazar == NULL){
			marco_a_reemplazar = solo_buscar_usado_modificado(marcos,PUNTERO_CLOCK);//Tercera pasada - Se repite paso 1;
			if (marco_a_reemplazar == NULL){
				marco_a_reemplazar = buscar_usado_modificado_cambia(marcos,PUNTERO_CLOCK);//Tercera pasada De ser necesario Se repite paso 2;
			}
		}
	}

	return marco_a_reemplazar;
}

Marco* buscar_usado_modificado_cambia(Marcos* marcos,int PUNTERO_CLOCK){
	bool condicion_busqueda(Marco* marco){
		if(marco->MARCOID < PUNTERO_CLOCK){
			return false;
		}
		else{
			if (marco->USADA == 0 && marco->MODIFICADA == 1){
				return true;
			}
			else{
				marco->USADA = 0;
				return false;
			}
		}
	}

	bool condicion_busqueda_siguiente(Marco* marco){
		if(marco->MARCOID < PUNTERO_CLOCK){
			if (marco->USADA == 0 && marco->MODIFICADA == 1){
				return true;
			}
			else{
				marco->USADA = 0;
				return false;
			}
		}
		else{
			return false;
		}
	}

	Marco* MARCO_A_REEMPLAZAR;

	MARCO_A_REEMPLAZAR = list_find(marcos->lista,(void*)condicion_busqueda);
	if (MARCO_A_REEMPLAZAR == NULL){
		//aplicar el algoritmo en el resto de la lista
		MARCO_A_REEMPLAZAR = list_find(marcos->lista,(void*)condicion_busqueda_siguiente);
	}

	return MARCO_A_REEMPLAZAR;
}

Marco* solo_buscar_usado_modificado(Marcos* marcos,int PUNTERO_CLOCK){
	bool condicion_busqueda(Marco* marco){
		if(marco->MARCOID < PUNTERO_CLOCK){
			return false;
		}
		else{
			if (marco->USADA == 0 && marco->MODIFICADA == 0){
				return true;
			}
			else{
				return false;
			}
		}
	}

	bool condicion_busqueda_siguiente(Marco* marco){
		if(marco->MARCOID < PUNTERO_CLOCK){
			if (marco->USADA == 0 && marco->MODIFICADA == 0){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}

	Marco* MARCO_A_REEMPLAZAR;

	MARCO_A_REEMPLAZAR = list_find(marcos->lista,(void*)condicion_busqueda);
	if (MARCO_A_REEMPLAZAR == NULL){
		//aplicar el algoritmo en el resto de la lista
		MARCO_A_REEMPLAZAR = list_find(marcos->lista,(void*)condicion_busqueda_siguiente);
	}

	return MARCO_A_REEMPLAZAR;
}

int obtener_index(Marcos* marcos,int IDMARCO){
	int index = -1;

	bool condicion_busqueda(Marco* marco){
		index++;
		return marco->MARCOID == IDMARCO;
	}

	list_find(marcos->lista,(void*)condicion_busqueda);

	return index;
}

void reemplazar_marcos(Marcos* marcos,Marco* marco_old,Marco* marco_new){
	Marco* marco = buscar_marco_idMarco(marcos,marco_old->MARCOID);
	marco->PAGINAID = marco_new->PAGINAID;
	marco->USADA = 1;
	marco->MODIFICADA = 0;
	//int index = obtener_index(marcos,marco_old->MARCOID);
	//list_replace_and_destroy_element(marcos->lista,index,marco_new,(void*)destroy_marco);
}

Marco* buscar_marco_reemplazo_cambiar_usados(Marcos* marcos,int PUNTERO_CLOCK){
	Marco* MARCO_A_REEMPLAZAR;

	bool condicion_busqueda(Marco* marco){
		if(marco->MARCOID < PUNTERO_CLOCK){
			return false;
		}
		else{
			if (marco->USADA == 1){
				marco->USADA = 0;
				return false;
			}
			else{
				return true;
			}
		}
	}

	bool condicion_busqueda_siguiente(Marco* marco){
		if(marco->MARCOID < PUNTERO_CLOCK){
			if (marco->USADA == 1){
				marco->USADA = 0;
				return false;
			}
			else{
				return true;
			}
		}
		else{
			return false;
		}
	}

	MARCO_A_REEMPLAZAR = list_find(marcos->lista,(void*)condicion_busqueda);
	if (MARCO_A_REEMPLAZAR == NULL){
		//aplicar el algoritmo en el resto de la lista
		MARCO_A_REEMPLAZAR = list_find(marcos->lista,(void*)condicion_busqueda_siguiente);
	}

	return MARCO_A_REEMPLAZAR;
}

int obtener_nueva_posicion_puntero(Marcos* marcos,Marco* marco_seleccionado){
	int maximo = list_size(marcos->lista);

	int index_marco_seleccionado = obtener_index(marcos,marco_seleccionado->MARCOID);

	int index_marco_siguiente;

	if (index_marco_seleccionado == maximo -1){
		index_marco_siguiente = 0;
	}
	else{
		index_marco_seleccionado++;
		index_marco_siguiente = index_marco_seleccionado;
	}

	Marco* marco_siguiente = list_get(marcos->lista,index_marco_siguiente);

	return marco_siguiente->MARCOID;
}

void ordenar_idmarco(Marcos* marcos){
	bool condicion_sort(Marco* marco1,Marco* marco2){
		return marco1->MARCOID < marco2->MARCOID;
	}

	//Ordeno por MarcoID
	list_sort(marcos->lista,(void*)condicion_sort);
}

void borrar_marco(Marcos* marcos,int PID){
	//no borrar

	bool condicion_busqueda(Marco* marco){
		return marco->PID == PID;
	}

	int cantidad_a_borrar = list_count_satisfying(marcos->lista,(void*)condicion_busqueda);
	int index;
	for(index = 0;index <= cantidad_a_borrar; index++){
		list_remove_and_destroy_by_condition(marcos->lista,(void*)condicion_busqueda,(void*)destroy_marco);
	}
}


//Funcion utilizada para cadena "PID1, PID2, etc"
char* recorrer_tabla_marcos(Marcos* marcos){
	char* cadena_PID = string_new();

	void funcion_closure(Marco* marco){
		char* MARCOID = string_itoa(marco->MARCOID);
		char* PID = string_itoa(marco->PID);
		char* NRO_PAG = string_itoa(marco->PAGINAID);
		char* USADA = string_itoa(marco->USADA);
		char* MODIFICADA = string_itoa(marco->MODIFICADA);
		string_append(&cadena_PID,MARCOID);
		string_append(&cadena_PID,"             ");
		string_append(&cadena_PID,"PID");
		string_append(&cadena_PID,PID);
		string_append(&cadena_PID,"     ");
		string_append(&cadena_PID,NRO_PAG);
		string_append(&cadena_PID,"               ");
		string_append(&cadena_PID,USADA);
		string_append(&cadena_PID,"          ");
		string_append(&cadena_PID,MODIFICADA);
		string_append(&cadena_PID,"\n");
	}

	list_iterate(marcos->lista,(void*)funcion_closure);

	return cadena_PID;
}

//Funcion utilizada para cadena "PID1, PID2, etc"
char* recorrer_tabla_marcos_un_pid(Marcos* marcos,int PID){
	char* cadena_PID = string_new();

	void funcion_closure(Marco* marco){
		if (marco->PID == PID){
			char* MARCOID = string_itoa(marco->MARCOID);
			char* PID = string_itoa(marco->PID);
			char* NRO_PAG = string_itoa(marco->PAGINAID);
			char* USADA = string_itoa(marco->USADA);
			char* MODIFICADA = string_itoa(marco->MODIFICADA);
			string_append(&cadena_PID,MARCOID);
			string_append(&cadena_PID,"             ");
			string_append(&cadena_PID,"PID");
			string_append(&cadena_PID,PID);
			string_append(&cadena_PID,"     ");
			string_append(&cadena_PID,NRO_PAG);
			string_append(&cadena_PID,"               ");
			string_append(&cadena_PID,USADA);
			string_append(&cadena_PID,"          ");
			string_append(&cadena_PID,MODIFICADA);
			string_append(&cadena_PID,"\n");
		}
	}

	list_iterate(marcos->lista,(void*)funcion_closure);

	return cadena_PID;
}
