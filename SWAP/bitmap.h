#ifndef BITMAP_H_
#define BITMAP_H_

//#include "programa.h"

typedef struct bitmap Bitmap;

struct bitmap {
	int cant_paginas;
	int* mapa;
	int size_mapa;
};

Bitmap* new_bitmap(int cant_paginas);

void set_bit(Bitmap* bitmap,int pagina);

void set_pool_bit(Bitmap* bitmap,int start,int end);

void clear_pool_bit(Bitmap* bitmap,int start,int end);

void clear_bit(Bitmap* bitmap,int pagina);

int get_bit(Bitmap* bitmap,int pagina);

int controlar_si_entra_contiguo(Bitmap* bitmap,int cant_pag_necesarias);

int controlar_solo_si_entra(Bitmap* bitmap,int cant_pag_necesarias);

char* mostrar_mapa(Bitmap* bitmap);

//int controlar_si_entra(Bitmap* bitmap,Programa* programa,int cant_pag_usadas);

#endif /* BITMAP_H_ */
