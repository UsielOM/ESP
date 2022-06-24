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
#include <stdio.h>
#include <string.h>

#include "censo.h"



main(int argc, char* argv[]) {

	int cola_cg, cola_gc, pid;
	char opcion[256];
	int ncar;
	struct mensaje mensaje;
	key_t llave;
	enum {NO, SI} recibir = NO;


	llave = ftok(FICHERO_LLAVE, CLAVE_CLIENTE_GESTOR);
	if ((cola_cg = msgget(llave, 0666)) == -1)
	{
		perror("msget");
		exit(-1);
	}

	llave = ftok(FICHERO_LLAVE, CLAVE_GESTOR_CLIENTE);
	if ((cola_gc = msgget(llave, 0666)) == -1)
	{

		perror("msget");
		exit(-1);
	}


	mensaje.pid = pid = getpid();
	while (1)
	{
		printf("Orden: ");
		fflush(stdout);
		fgets(opcion, sizeof(opcion), stdin);
		switch (opcion[0])
		{
		case 'l':
			recibir = SI;
			mensaje.orden = LISTAR;
			msgsnd(cola_cg, &mensaje, LONGITUD, 0);
			break;
		case 'a':
			recibir = SI;
			printf("\tNombre: ");
			ncar = strlen(fgets(mensaje.datos.persona.nombre, MAX_NOMBRE, stdin));

			mensaje.datos.persona.nombre[ncar - 1] = '\0';
			printf("\tDireccion: ");
			ncar = strlen(fgets(mensaje.datos.persona.direccion, MAX_DIRECCION, stdin));
			mensaje.datos.persona.direccion[ncar - 1] = '\0';

			printf("\tTelefono: ");
			ncar = strlen(fgets(mensaje.datos.persona.direccion, MAX_TELEFONO, stdin));
			mensaje.datos.persona.telefono[ncar - 1] = '\0';
			mensaje.orden = ANNADIR;
			msgsnd(cola_cg, &mensaje, LONGITUD, 0);
			break;

		case 'f':
			recibir = NO;
			mensaje.orden = FIN;
			msgsnd(cola_cg, &mensaje, LONGITUD, 0);
		case 's':
			exit(0);
		default:
			recibir = NO;
			printf("Opcion [%c] erronea. \n");
			printf("Opciones disponibles: \n");
			printf("\ta - añadir registros. \n");
			printf("\tf - parar el programa gestor. \n");
			printf("\tl - visualizar todos los registros. \n");
			printf("\ts - parar el programa cliente . \n");
			
		}
		if(recibir == SI)
			do {
				msgrcv(cola_cg, &mensaje, LONGITUD, pid, 0);
				switch (mensaje.orden)
				{
				case FIN:
					break;
				case LISTAR:
					printf("\n\tNombre: %s\n",
						mensaje.datos.persona.nombre);
					printf("\tDireccion: %s\n",
						mensaje.datos.persona.direccion);
					printf("\tTelefono: %s\n", mensaje.datos.persona.telefono);
						break;
				case ERROR:
					printf("Mensaje de error recibido. \n");
					break;
				default:
					printf("Tipo de mensaje desconocido [%d]\n", mensaje.orden);
					break;
				}
			} while (mensaje.orden != FIN && mensaje.orden != ERROR);
	}



}
