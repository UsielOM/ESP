
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
#include "errores.h"
#include <errno.h>
#include "colas.h"
#include <time.h>




TYPEDEF STRUCT{

	int sec;
char datos[11];

}mensaje_t;

void* cod_del_emisor(void* arg)
{
	cola_t* cola = (cola_t*)arg;

	struct timespec retardo = { 0, 250000000 };
	int i;
	int error;
	mensaje_t msg;
	for (i = 1; i <= 100; i++)
	{
		msg.sec = 1;
		if (i % 10 == 0)
			strncpy(msg.datos, "URGENTE", sizeof(msg.datos));
		else
			strncpy(msg.datos, "normal", sizeof(msg.datos));
		error = poner_en_cola(cola, (elem_t*)msg, i % 10 == 0 ? 2 : 1);
		if (error)
			error_fatal(error, "poner_en_la_cola");
		error = nanosleep(&retardo, NULL);
		if (error == -1)
			error_ fatal(errno, "nanosleep");

	}
	++msg.sec;
	strncpy(msg.datos, "FIN", sizeof(msg.datos));
	poner_en_la_cola(cola, (elem_t*)&msg, 0);
	printf("Fin de la emisor. \n");
	pthread_exit(0);


}

void* cod_del_receptor(void* arg)
{
	cola_t* cola = (cola_t*)arg;
	struct timespec retardo = { 1,0 };
	int prio;
	int error;
	mensaje_t msg;
	do {
		quitar_de_la_cola(cola, (elem_t*)&msg, &prio);
		if (error)
			error_fatal(error, "quitar_de_la_cola");
		printf("Mensaje %d %s con prioridad %d\n", msg.sec, msg.datos, prio);
		error = nanosleep(&retardo, NULL);
		if (error == -1)
			error_fatal(errno, "nanosleep");

	} while (strcmp(msg.datos, "FIN") != 0);
	printf("Fin del receptor.\n");
	pthread_exit(0);

}



main(int argc, char* argv[])
{
	cola_t cola = INICIALIZAR_LA_COLA(32, mensaje_t);
	pthread_t emisor, receptor;
	int error;

	error = pthread_create(&emisor, NULL, cod_del_emisor, (void*)&cola);
	if (error)
		error_fatal(error, "pthread_create");
	error = pthread_create(&receptor, NULL, cod_del_receptor, (void*)cola);
	if (error)
		error_fatal(error, "pthread_create");

	error = pthread_join(emisor, NULL);
	if (error)
		error_fatal(error, "pthread_join");
	error = pthread_join(receptor, NULL);
	if (error)
		error_fatal(error, "pthread_join");
	printf("Fin del hilo principal. \n");

}



