#include "visor.h"
#include <commons/string.h>
#include <unistd.h>

int main(char argc,char* argv[]){

	Visor* visor = new_visor(argv[1]);

	printf("Path del log: %s\n",visor->PATH);

	char* comando = string_new();
	string_append(&comando,"tail -n 50 ");
	string_append(&comando,visor->PATH);

	while(1){
		usleep(1000000);
		system("clear");
		system(comando);
	}

	return 0;
}
