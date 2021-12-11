#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#define I_AM_ALIVE SIGUSR2

void killAll(int sig){
    printf("\nSIGUSR1 ricevuto, uccisione dei processi in corso...\n");
    remove("PipeP1");
    remove("SocketP2");
    remove("FileCondivisoP3.txt");
    remove("DFsocket"); //Nome della socket gestita dal processo 'Decision Function'
    kill(0, SIGKILL);
}

int main(int argc,char* argv){
    signal(SIGUSR1,killAll);
    signal(SIGINT, killAll);

    /* Ignoro il segnale I_AM_ALIVE */
    signal(I_AM_ALIVE, SIG_IGN);
    pause();
}
