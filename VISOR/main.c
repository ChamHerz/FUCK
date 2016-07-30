#include "visor.c"
#include <commons/string.h>
#include <unistd.h>

int main(char argc,char* argv[]){

	Visor* visor = new_visor(argv[1]);

	printf("Path del log: %s\n",visor->PATH);

	char* comando = string_new();

<<<<<<< HEAD
	string_append(&comando,"tail -50");
=======
	string_append(&comando,"tail -n 50 ");
>>>>>>> 9043b9c51c5f1ee318e2ee09047c066822a7da77
	string_append(&comando,visor->PATH);

	while(1){
		usleep(1000000);
		system("clear");
		system(comando);
	}

	return 0;
}
