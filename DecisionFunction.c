#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */
#include <signal.h>
#include <unistd.h>
// #define I_AM_ALIVE SIGUSR2
#define SYSTEM_LOG "log/system_log.txt"
#define VOTED_OUTPUT "log/voted_output.txt"

/* Dichiarazione dei file di log */
FILE* system_log; //scrivere sopra SUCCESSO o FALLIMENTO
FILE* voted_output; //scrivere sopra i valori ricevuti da P1, P2, P3

void main(){
    system_log = fopen(SYSTEM_LOG, "w"); //li prede da run e li definisce sopra tra le librerie
    voted_output = fopen(VOTED_OUTPUT, "w");

    /* Handler per interruzione con CONTROL-C */
    signal(SIGINT, SIG_IGN); //?????????????

    /* Ignoro i segnali SIGUSR1 e I_AM_ALIVE */
    //signal(SIGUSR1, SIG_IGN);
    //signal(I_AM_ALIVE, SIG_IGN);

    /* Creazione della socket DFsocket */
    int serverFd, clientFd1, clientFd2, clientFd3, serverLen, clientLen;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr *serverSockAddrPtr;
    struct sockaddr_un clientUNIXAddress;
    struct sockaddr *clientSockAddrPtr;

    serverSockAddrPtr = (struct sockaddr *)&serverUNIXAddress;
    serverLen = sizeof(serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr *)&clientUNIXAddress;
    clientLen = sizeof(clientUNIXAddress);

    serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy(serverUNIXAddress.sun_path, "DFsocket"); /* Setto il nome della socket */
    unlink("DFsocket");                             /* Rimuovo il file se già esiste */
    bind(serverFd, serverSockAddrPtr, serverLen);
    listen(serverFd, 4);

    /* Eseguo 3 accept, una per ciascuna connessione in ingresso */
    clientFd1 = accept(serverFd, clientSockAddrPtr, &clientLen);
    clientFd2 = accept(serverFd, clientSockAddrPtr, &clientLen);
    clientFd3 = accept(serverFd, clientSockAddrPtr, &clientLen);
    char firstResult[6], secondResult[6], thirdResult[6];
    int count = 0;

    while (1)
    {
        /* Recezione dei dati */
        read(clientFd1,firstResult,6);
        read(clientFd2,secondResult,6);
        read(clientFd3,thirdResult,6);

        /* Scrittura su voted_output dei valori:
           I dati sono scritti in terne separate da uno '\n' */
        fwrite(firstResult, 1, strlen(firstResult), voted_output);
        fwrite(", ", 1, 1, voted_output);
        fwrite(secondResult, 1, strlen(firstResult), voted_output);
        fwrite(", ", 1, 1, voted_output);
        fwrite(thirdResult, 1, strlen(firstResult), voted_output);
        fwrite("\n", 1, 1, voted_output);
        fclose(voted_output);

        /* Valutazione dei risultati tramite comparazione */
        if(strcmp(firstResult, secondResult) == 0)
            count++;
        if(strcmp(firstResult, thirdResult) == 0)
            count++;
        if(strcmp(secondResult, thirdResult) == 0)
            count++;
        if(count > 0) {
            /* Caso di successo (almeno due strighe uguali) */
            fwrite("SUCCESSO\n", 1, 9, system_log);
            fclose(system_log);
            kill(0, I_AM_ALIVE);
        }
        else {
            /*caso di fallimento*/
            fwrite("FALLIMENTO\n", 1, 11, system_log);
            fclose(system_log);
            kill(0,SIGUSR1);
        }
        count = 0;

        voted_output = fopen(VOTED_OUTPUT, "a");
        system_log = fopen(SYSTEM_LOG, "a");
    }
    close(clientFd1);
    close(clientFd2);
    close(serverFd);
}
