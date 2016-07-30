#ifndef PRIMITIVAS_H_
#define PRIMITIVAS_H_

#include <parser/parser.h>
#include "cpu.h"

void parsear_linea(Cpu* cpu,char* cadena_instruccion);

int agregar_definicion_variable(Pcb* pcb,t_nombre_variable variable);

Address* obtener_direccion_variable(Pcb* pcb,t_nombre_variable variable);

void asignar_valor_variable(Cpu* cpu,Address* address,t_valor_variable valor_variable);

int dereferenciar_direccion(Cpu* cpu,Address* address);

void finalizar_programa(Cpu* cpu, Pcb* pcb);

void enviar_wait(Cpu* cpu,char semaforo);

void enviar_signal(Cpu* cpu,char semaforo);

int obtener_valor_variable_global(Cpu* cpu,char* variable_global);

void asignar_valor_variable_global(Cpu* cpu,char* variable_global,int valor);

void ejecutar_etiqueta(Cpu* cpu,char* nombre_etiqueta);

void imprimir(Cpu* cpu,int valor_variable);

void imprimirTexto(Cpu* cpu, char* texto);

void enviar_entrada_salida(Cpu* cpu, char* dispositivo, int tiempo);

void ejecutar_llamadaConRetorno(Cpu* cpu,char* etiqueta,int puntero_retorno);

void ejecutar_retornar(Cpu* cpu,int retorno);

#endif /* PRIMITIVAS_H_ */
