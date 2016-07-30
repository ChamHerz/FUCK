#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>
#include "programa.h"
#include "ansisop.h"

Bitmap* new_bitmap(int cant_paginas){
	int size_vector = cant_paginas / 32;
	if ( cant_paginas % 32 != 0 ){
		size_vector++;
	}
	Bitmap* b;
	b->mapa = (int*)calloc(size_vector,sizeof(int)); //crea el vector de int y los pone en 0;
	b->size_mapa = size_vector;
	b->cant_paginas = cant_paginas;
	return b;
}

void set_bit(Bitmap* bitmap,int pagina){
	bitmap->mapa[pagina/32] |= 1 << (pagina%32);
}

void clear_bit(Bitmap* bitmap,int pagina){
	bitmap->mapa[pagina/32] &= ~(1 << (pagina%32));
}

int get_bit(Bitmap* bitmap,int pagina){
	return ((bitmap->mapa[pagina/32] & (1 << (pagina%32))) != 0);
}

int controlar_solo_si_entra(Bitmap* bitmap,int cant_pag_necesarias){
	int programa_entro = 0;
	int pag_actual = 0;
	int sumador_pag = 0;

	while (programa_entro == 0 && pag_actual < bitmap->cant_paginas){
		if(get_bit(bitmap,pag_actual)==0){
			sumador_pag++;
		}
		if (sumador_pag == cant_pag_necesarias){
			programa_entro = 1;
		}
		pag_actual++;
	}

	return programa_entro;
}

int controlar_si_entra_contiguo(Bitmap* bitmap,int cant_pag_necesarias){
	int sumador_pag = 1;
	int i=0;
	while(sumador_pag <= cant_pag_necesarias & i < bitmap->cant_paginas){
		if(get_bit(bitmap,i)==0){
			sumador_pag++;
		}
		else{
			sumador_pag = 1;
		}
		i++;
	}
	sumador_pag--;
	printf("\nbitmap->size_mapa: %d\n",bitmap->cant_paginas);
	printf("\nprograma->CANT_PAG: %d\n",cant_pag_necesarias);
	printf("\nsumador_pag: %d\n",sumador_pag);
	if (sumador_pag == cant_pag_necesarias){//Significa que entra en este bloque
		printf("pag de inicio: %d\n",i - sumador_pag);
		return 1;
	}

	return 0;
}

int set_bitmap(Bitmap* bitmap,Ansisop* ansisop, Programa* programa, int cant_pag_necesarias){
	int sumador_pag = 1;
	int i=0;
	while(sumador_pag <= cant_pag_necesarias & i < bitmap->cant_paginas){
		if(get_bit(bitmap,i)==0){
			sumador_pag++;
		}
		else{
			sumador_pag = 1;
		}
		i++;
	}
	sumador_pag--;
	printf("\nbitmap->size_mapa: %d\n",bitmap->cant_paginas);
	printf("\nprograma->CANT_PAG: %d\n",cant_pag_necesarias);
	printf("\nsumador_pag: %d\n",sumador_pag);
	if (sumador_pag == cant_pag_necesarias){//Significa que entra en este bloque
		programa->START_PAG = i - sumador_pag;
		programa->CANT_PAG = sumador_pag;
		printf("pag de inicio: %d\n",i - sumador_pag);
		set_pool_bit(bitmap,programa->START_PAG,programa->START_PAG + programa->CANT_PAG);
		return 1;
	}
	return 0;
}

void set_pool_bit(Bitmap* bitmap,int start,int end){
	int i;
	for (i=start;i<end;i++){
		set_bit(bitmap,i);
		printf("Bit %d cambiado\n",i);
	}
}

void clear_pool_bit(Bitmap* bitmap,int start,int end){
	int i;
	for (i=start;i<end;i++){
		clear_bit(bitmap,i);
		printf("Bit %d cambiado a 0\n",i);
	}
}

char* mostrar_mapa(Bitmap* bitmap){
	char* mapa = malloc(sizeof(bitmap->cant_paginas));
	char* cadena_final = string_new();
	int i;
	for(i = 0; i < bitmap->cant_paginas;i++){
		if (get_bit(bitmap,i)==1){
			//mapa[i] = '█';
			string_append(&cadena_final,"█");
		}
		else{
			string_append(&cadena_final,"░");
			//mapa[i] = '░';
		}
	}
	//░";
	return cadena_final;

	//char a = '█';
	//int f = a;
	//printf("%d",f);
	//char* cadena = "█";
	//return string_repeat(01001130554,110);
	//return string_repeat('\210',110);
	//return cadena;
}
