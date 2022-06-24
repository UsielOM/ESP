#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "censo.h"



main(int argc, char* argv[]) {

	int cola_cg, cola_gc;
	struct mensaje mensaje;
	key_t llave;
	FILE* pf;


	if (argc != 2) {

		fprintf(stderr, "Forma de uso: %s fichero.\n", argv[0]);
		exit(-1);
	}

	llave = ftok(FICHERO_LLAVE, CLAVE_CLIENTE_GESTOR);
	if ((cola_cg = msgget(llave, IPC_CREAT | 0666)) == -1)
	{
		perror("msget");
		exit(-1);

	}
	llave = ftok(FICHERO_LLAVE, CLAVE_GESTOR_CLIENTE);
	if ((cola_gc = msgget(llave, IPC_CREAT | 0666)) == -1)
	{
		perror("msget");
		exit(-1);
	}

	while (1)
	{
		msgrcv(cola_cg, &mensaje, LONGITUD, 0, 0);
		switch (mensaje.orden)
		{
		case LISTAR:
			if ((pf = fopen(argv[1], "r")) == NULL) {

				mensaje.orden = ERROR;
				msgsnd(cola_gc, &mensaje, LONGITUD, 0);
				break;
			}
			while (fread (&mensaje.datos.persona, 
				sizeof (struct persona ), 1, pf) == 1)
				msgsnd(cola_gc, &mensaje, LONGITUD, 0);
				fclose(pf);
				mensaje.orden = FIN;
				msgsnd(cola_gc, &mensaje, LONGITUD, 0);
				break;

		case ANNADIR:
			if ((pf = fopen(argv[1], "a")) == NULL) {
				mensaje.orden = ERROR;
				msgsnd(cola_gc, &mensaje, LONGITUD, 0);
				break;
			}
			fwrite(&mensaje.datos.persona, sizeof(struct persona), 1, pf);
			fclose(pf);

			mensaje.orden = FIN;
			msgsnd(cola_gc, &mensaje, LONGITUD, 0);
			break;
			
		case FIN:
			msgctl(cola_cg, IPC_RMID, 0);
			msgctl(cola_gc, IPC_RMID, 0);
			printf("Programa gestor parado a peticion de un cliente .\n");
			exit(0);


		default:
			mensaje.orden = ERROR;
			msgsnd(cola_gc, &mensaje, LONGITUD, 0);
			break;
		}
	}
}



