/*
 * file.h
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#ifndef FILE_H_
#define FILE_H_

#define EVENT_SIZE ( sizeof(struct inotify_event) + 24)
#define BUF_LEN ( 1024 * EVENT_SIZE )

int iniciar_escucha_archivo(char* path);

#endif /* FILE_H_ */
