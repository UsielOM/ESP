#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[]){
    int pet;
    char resp;
    char *host;
    int puerto;
    int sd;
    int addrlen;
    struct sockaddr_in myaddr;
    struct sockaddr_in svaddr;
    struct sockaddr_in claddr;
    struct hostent *hp;

    if(argc !=4){
        fprintf(stderr, "Error, uso %s <host> <puerto> <numero> \n", argv[0]);
        exit(1);
    }
    host = argv[1];
    puerto = atoi(argv[2]);

    svaddr.sin_family = AF_INET;
    hp = gethostbyname(host);
    if (hp == NULL){
        fprintf(stderr, "No se encontro %s en /etc/hosts \n", host);
        exit(1);
    }
    svaddr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    svaddr.sin_port = puerto;

    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = 0;
    myaddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr_in))== - 1){
        perror("bind");
        exit(1);
    }

    pet = atoi(argv[3]);
    addrlen = sizeof(svaddr);

    if(sendto(sd, &pet, sizeof(resp), 0, (struct sockaddr *)&svaddr, addrlen) == -1){
        perror("sendto");
        exit(1);
    }
    if(recvfrom(sd, &resp, sizeof(resp), 0, (struct sockaddr *)&claddr, &addrlen)){
        perror("recvfrom");
        exit(1);
    }
    printf("E numero %d es ", pet);
    if(resp == 'i') printf("impar \n");
    else printf("par \n");

    close(sd);
    return 0;
}
