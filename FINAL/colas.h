#ifndef _COLAS_H_
#define _COLAS_H_
#include <sys/types.h>
#include <pthread.h>
typedef char elem_t;
struct elem_c {
unsigned prio;
elem_t* elem;
};
typedef struct cola {
unsigned capacidad;
size_t tam_elem;
struct elem_c* ptr_base;
int primero;
int ultimo;
unsigned num_elem;
pthread_mutex_t cerrojo;
pthread_cond_t hay_elementos;
pthread_cond_t hay_sitio;
}colas_t;
#define INICIALIZAR_LA_COLA(capacidad, tipo_elem){
(capacidad),\
sizeof (tipo_elem),\
(struct elem_c *)reservar_memoria (capacidad, sizeof(tipo_elem)),\
0,\
0, \
0, \
PTHREAD_MUTEX_INITIALIZER,\
PTHREAD_COND_INITIALIZER, \
PTHREAD_COND_INITIALIZER\
}
int poner_en_la_cola(cola_t* cola, elem_t* elem, unsigned prio);
int quitar_de_la_cola(cola_t* cola, elem_t* elem, unsigncd* prio);
#endif // !1
