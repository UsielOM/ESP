#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#define SEM_HIJO 0
#define SEM_PADRE 1

int main (int argc, char *argv[]){
    int i = 10, semid, pid;
    struct sembuf operacion;
    key_t llave;

    llave = ftok (argv[0], 'K');
    if((semid = semget (llave, 2, IPC_CREAT | 0600)) == -1 ){
        perror ("semget");
        exit (-1);
    }

    semctl(semid, SEM_HIJO, SETVAL, 0);
    semctl(semid, SEM_PADRE, SETVAL, 1);

    if ((pid = fork()) == -1){
        perror("fork");
        exit(-1);
    } else if (pid == 0){
        while (i){
            operacion.sem_num = SEM_HIJO;
            operacion.sem_op = -1;
            operacion.sem_flg = 0;
            semop (semid, &operacion, 1);

            printf("PROCESO HIJO: %d\n", i--);

            operacion.sem_num= SEM_PADRE;
            operacion.sem_op=1;
            semop (semid, &operacion, 1);
        }
        semctl (semid, 0, IPC_RMID, 0);
    }else {
        operacion.sem_flg=0;
        while(i){
            operacion.sem_num = SEM_PADRE;
            operacion.sem_op = -1;

            semop (semid, &operacion, 1);
            printf("PROCESO PADRE: %d\n", i--);
            operacion.sem_num = SEM_HIJO;
            operacion.sem_op = 1;
            semop (semid, &operacion, 1);
        }

        semctl (semid, 0, IPC_RMID, 0);
    }
}