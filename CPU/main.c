#include "cpu.h"

#include <signal.h>
#include <unistd.h>
#include <commons/process.h>

#define PATH_CPU "cpu.conf"

void hilo_sisgur(Cpu* cpu){

	void controlador (int numeroSenhal)
	{
		printf ("Recibido la señal SIGUSR1 \n");
		printf ("Verifico si puedo irme\n");
		cpu->evento_cierre_por_SISGUR1 = 1;

		/* Se pone controlador por defecto para ctrl-c */
		signal (SIGUSR1, SIG_DFL);
	}

	void abortar(int numeroSenhal){
		int dato_enviar = 532; //kill
		escribir_descriptor(cpu->DESCRIPTOR_NUCLEO,(char*)&dato_enviar,sizeof(int));
		char* comando = string_new();
		string_append(&comando,"kill  ");
		char* PID = string_itoa(process_getpid());
		string_append(&comando,PID);
		system(comando);
		//pthread_cancel(cpu->hilo_para_nucleo); //mato al hilo principal
		//pthread_exit(NULL); //mato al hilo actual
	}

	if (signal (SIGUSR1, controlador) == SIG_ERR)
	{
		perror ("No se puede cambiar signal");
	}

	if (signal (SIGINT, abortar) == SIG_ERR){
		perror ("No se puede cambiar signal");
	}

	while (1)
		pause ();
}

main(){

	Cpu* cpu = new_cpu();

	set_path_conf(cpu,PATH_CPU);

	cargar_archivo_configuracion(cpu);

	crear_hilo_un_para_sisgur(hilo_sisgur,cpu);

	cpu_inciar(cpu);

	return 0;
}
