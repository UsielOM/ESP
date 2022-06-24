#ifndef _CENSO_
#define _CENSO_

#define MAX_NOMBRE 61
#define MAX_DIRECCION 121
#define MAX_TELEFONO 11



struct persona
{
	char nombre[MAX_NOMBRE];
	char direccion[MAX_DIRECCION];
	char telefono[MAX_TELEFONO];

};


struct mensaje
{
	long pid;
	int orden;
	union {
		struct persona persona;
	}datos;

};


#define LONGITUD (sizeof (struct mensaje) - sizeof (long))

#define LISTAR 1
#define ANNADIR 2
#define FIN 3
#define ERROR 4


#define FICHERO_LLAVE "censo.h"
#define CLAVE_CLIENTE_GESTOR 'K'
#define CLAVE_GESTOR_CLIENTE 'L'
#endif // !1

