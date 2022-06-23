#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int pet;
char resp;
int puerto;
int sd;
int addrlen;
int cc;
char *nombre_clt;
int dirlen;
struct sockaddr_in myaddr;
struct sockaddr_in claddr;
struct hostent *host_clt;


void aborta()
{
   printf("....abortando el proceso servidor \n");
   close(sd);
   exit(1);

}

int main(int argc, char *argv[])
{
  
   signal(SIGINT, aborta);

   if (argc != 2){
   fprintf(stderr,"Error, uso: %s <puerto> \n",argv[0]);
   exit(1);
   }
puerto = atoi(argv[1]);

myaddr.sin_family = AF_INET;
myaddr.sin_addr.s_addr = INADDR_ANY;
myaddr.sin_port = puerto;

if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){

     perror("socket");
     exit(1);

}

if(bind(sd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr_in *))== - 1){
        perror("bind");
        exit(1);
}

while(1) {
addrlen = sizeof(struct sockaddr_in);
cc = recvfrom(sd, &pet, sizeof(pet), 0, (struct sockaddr *)&claddr, &addrlen);
if ( cc == -1){

    perror("recv");
    exit(1);
}

dirlen = sizeof(claddr.sin_addr.s_addr);
host_clt = gethostbyaddr((void*)&claddr.sin_addr.s_addr, dirlen , AF_INET); 
if (host_clt == NULL)
nombre_clt = "desconocido";
else
nombre_clt = host_clt->h_name;
printf("El mensaje [%d] fue recibido de: %s \n" ,pet, nombre_clt);


if((pet % 2) == 0) resp= 'p';
else resp='i';

if (sendto(sd, &resp, sizeof(resp), 0, (struct sockaddr *)&claddr, addrlen)== -1){

    perror("sendto");
    exit(1);
      }
   }
   close(sd);

   return 0;
}