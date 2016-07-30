#include "file.h"
#include <sys/inotify.h>
#include <stdio.h>

int iniciar_escucha_archivo(char* path){
	printf("escucha iniciada en: %s \n", path);

	int file_descriptor = inotify_init();
	int whath_descriptor;
	int length;
	char buffer[BUF_LEN];

	if (file_descriptor < 0){
		printf("error en inotify");
	}

	whath_descriptor = inotify_add_watch(file_descriptor,path,IN_MODIFY);

	length = read(file_descriptor, buffer, BUF_LEN);

	if (length < 0){
		printf("error en el read de inotify");
	}

	int offset = 0;

	while (offset < length){
		struct inotify_event* evento = (struct inotify_event *) &buffer[offset];
		offset += sizeof(struct inotify_event) + evento->len;
	}

	inotify_rm_watch(file_descriptor,whath_descriptor);
	close(file_descriptor);

	return 0;
}
