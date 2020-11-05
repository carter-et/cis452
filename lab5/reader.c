#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

void exit_handler(int);

int main () {
    int shmId, *p, p1;
    char *shmPtr, *m_ptr, *reader[2];

    signal(SIGINT, exit_handler);
    key_t key = ftok("/tmp", 7);

    //create and attach
    if ((shmId = shmget(key, 4096, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0){
        perror("ERROR: shmget\n");
        exit(1);
    }
    if((shmId = shmat(shmId, 0, 0)) == (void*) -1){
        perror("issue with shma\nt");
        exit(1);
    }
    
    while(1){
        p = (int*)shmPtr;

        m_ptr = shmPtr + sizeof(int) * 2;
        reader[0] = m_ptr;
        printf("Recieved: %sn", reader[0]);
        m_ptr += *p++;
        reader[1] = m_ptr;

        sscanf(m_ptr, "%d", &p1);
        while(!p){
            sscanf(m_ptr, "%d", &p1);
        }
    }

    //detach and remove
    if(smhdt(shmPtr) < 0){
        perror("issue detaching\n");
        exit(1);
    }
    if(shmctl(shmId, IPC_RMID, 0) < 0){
        perror("issue removing\n");
        exit(1);
    }
    
    exit(0);
}

void exit_handler(int sigNum){
    printf("\n");
    exit(0);
}