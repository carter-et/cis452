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
    int shmId;
    char *shmPtr;
    char *buf = malloc(256 * sizeof(char));

    key_t key = ftok("tmp", 7);
    signal(SIGINT, exit_handler);

    //create and attach
    if ((shmId = shmget(key, 4096, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0){
        perror("ERROR: shmget\n");
        exit(1);
    }
    if((shmId = shmat(shmId, 0, 0)) == (void*) -1){
        perror("issue with shma\nt");
        exit(1);
    }

    //pointers for saving off diff mem address, where m_ptr is temp
    char *ptr1, *ptr2, *m_ptr;
    int mem_addr[2], mem1, mem2;

    //takes user input and stores it in shared mem
    while(1){
        printf("Enter message: ");
        fgets(buf, 256, stdin);

        m_ptr = shmPtr + sizeof(mem_addr);
        mem_addr[0] = 32;
        sprintf(m_ptr, "%s", buf);
        m_ptr += mem_addr[0];
        ptr1 = m_ptr;
        mem_addr[1] = sprintf(m_ptr, "1") + 1;
        m_ptr += mem_addr[1];
        ptr2 = m_ptr;
        sprintf(m_ptr, "1");

        memcpy(shmPtr, &mem_addr, sizeof(mem_addr));
        sscanf(ptr1, "%d", &mem1);
        sscanf(ptr2, "%d", &mem2);

        // continue to get input from user
        while (mem1 && mem2){
            sscanf(ptr1, "%d", &mem1);
            sscanf(ptr2, "%d", &mem2);
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