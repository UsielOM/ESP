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


struct elem_c* reserver_memoria(unsigned capacidad, size_t tam_elem) {

	int i;
	struct  elem_c* ptr_elem;

	ptr_elem = (struct elem_c*)malloc(capacidad * sizeof(struct elem_c));
	if (ptr_elem == NULL)
		error_fatal(errno, "malloc");
	for (i = 0; i < capacidad; i++)
	{
		ptr_elem[i].elem = (elem_t*)malloc(tam_elem);
		if (ptr_elem[i].elem == NULL)
			errno_fatal(errno, "malloc");
		
	}

	return ptr_elem;

}

int poner_en_la_cola(cola_t* cola, elem_t* elem, unsigned prio)
{

	int error;
	error = pthread_mutex_lock(&cola->cerrojo);
	if (error)
		error(error, "pthread_mutex_lock");
	while (cola->num_elem == cola->capacidad) {
		error = pthread_cond_wait(&cola->hay_sitito, &cola->cerrojo);
		if (error)
			error(error, "pthread_cond_wait");
	}


	cola->ptr_base[cola->ultimo].prio = prio;
	memcpy(cola->ptr_base[cola->ultimo].elem, elem, cola->tam_elem);
	cola->ultimo = (cola->ultimo + 1) % cola->capacidad;
	++cola->num_elem;


	error = pthread_cond_signal(&cola->hay_elementos);
	if (error)
		error(error, "pthread_cond_signal");
	error = pthread_mutex_unlock(&cola->cerrojo);
	if (error)
		error(error, "pthread_mutex_unlock");
	return 0;


}

int quitar_de_la_cola(cola_t* cola, elem_t* elem, unsigned* prio)
{
	int error;
	int i, j, max;
	struct elem_c aux;

	error = pthread_mutex_lock(&cola->cerrojo);
	if (error)
		error(error, "pthread_mutex_lock");
	while (cola->num_elem == 0)
	{
		error = pthread_cond_wait(&cola->hay_elemtos, &cola->cerrojo);
		if (error)
			error(error, "pthread_cond_wait");
	}

	i = max = cola->primero;
	do {
		if (cola->ptr_base[i].prio > cola->ptr_base[max].prio)
			max = i;
		i = (i + 1) % cola->capacidad;

	} while (i != cola->ultimo);

	memcpy(elem, cola->ptr_base[max].elem, cola->tam_elem);
	*prio = cola->ptr_bae[max].prio;

	if (max == cola->primero)
		cola->primero = (cola->primero + 1) % cola->capacidad;

	else {
		i = max;
		j = (max + 1) % cola->capacidad;
		aux = cola->ptr_base[i];
		while (j != cola->ultimo)
		{
			cola->ptr_base[i] = cola->ptr_base[j];
			i = j;
			j = (j + 1) % cola->capacidad;

		}
		cola->ptr_base[i] = aux;
		cola->ultimo = i;
	}
	
	--cola->num_elem;
	error = pthread_cond_signal(&cola->hay_sitio);
	if (error)
		error(error, "pthread_cond_signal");
	error = pthread_mutex_unlock(&cola->cerrojo);
	if (error)
		error(error, "pthread_mutex_unlock");
	return 0;
}



