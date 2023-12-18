#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/shm.h>
#define N 256

//Массив семафоров. Первый элемент доступ к критической секции, второй - разделяемой памяти, третий - количество читателей  
struct sembuf op;

char str[N];


int shmid, semid; 
char* smaddr; 

void reader(void){
    int s; 
    op.sem_op=-1; 
    op.sem_num=0; 
    semop(semid, &op, 1);
    op.sem_num=2; 
    op.sem_op=1;
    semop(semid, &(op), 1);
    s=semctl(semid, 2, GETVAL, NULL);
    if(s==1){
        op.sem_num=1; 
        op.sem_op=-1;
        semop(semid, &(op), 1);
    }
    op.sem_num=0; 
    op.sem_op=1;
    semop(semid, &op, 1);
    //read
    int i=0; 
    while(smaddr[i]) printf("%c", smaddr[i++]); 
    printf("\n"); 
    op.sem_num=0; 
    op.sem_op=-1;
    semop(semid, &op, 1);
    op.sem_num=2; 
    op.sem_op=-1;
    semop(semid, &(op), 1);
    s=semctl(semid, 2, GETVAL, s);
    if(s==0) {
        op.sem_num=1; 
        op.sem_op=1;
        semop(semid, &(op), 1);
    }
    op.sem_num=0; 
    op.sem_op=1;
    semop(semid, &op, 1);
}

int main(){
    key_t key=ftok("file.bin", 't');
    shmid=shmget(key, N, IPC_CREAT|0666);
    smaddr=shmat(shmid, NULL, 0);
    semid=semget(key,3, IPC_CREAT|0666 );
    op.sem_num=0;
    op.sem_flg=0;
    semctl(semid, 0, SETVAL, (int) 1); 
    semctl(semid, 1, SETVAL, (int) 1); 
    semctl(semid, 2, SETVAL, (int) 0); 
    reader();
    return 0;
}
