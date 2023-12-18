#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/shm.h>
#include <fcntl.h>
#define N 256

//Массив семафоров. Первый элемент доступ к критической секции, второй - разделяемой памяти, третий - количество читателей  

int shmid, semid, i=0; 
char* smaddr; 
void writer(){
    struct sembuf op;
    char str[N]; 
    char c; 
    op.sem_num=1;
    op.sem_flg=0;
    op.sem_op=-1;
    semop(semid,&op, 1);
    //write
    while((c=getchar())!='\n') smaddr[i++]=c; 
    smaddr[i++]='\0'; 
    op.sem_num=1;
    op.sem_op=1;
     semop(semid,&op, 1);
}

int main(){
    key_t key=ftok("file.bin", 't');
    shmid=shmget(key, N, IPC_CREAT|0666);
    smaddr=shmat(shmid, NULL, 0);
    semid=semget(key,3, IPC_CREAT|0666 );
    semctl(semid, 0, SETVAL, (int) 1); 
    semctl(semid, 1, SETVAL, (int) 1); 
    semctl(semid, 2, SETVAL, (int) 0); 
    writer();
    return 0;
}
