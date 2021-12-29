#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define I_AM_ALIVE SIGUSR2
#define SYSTEM_LOG "./log/system_log.txt"
#define VOTED_OUTPUT "./log/voted_output.txt"

/* Dichiarazione dei file di log */
FILE* system_log;
FILE* voted_output;

///////////////////////////////////////////////////////////////////
////////////////////////// Main //////////////////////////////////
/////////////////////////////////////////////////////////////////
void main() {

  /* Creo i due file di log */
  system_log = fopen(SYSTEM_LOG, "w");
  voted_output = fopen(VOTED_OUTPUT, "w");

  /* Handler per interruzione con CONTROL-C */
  signal(SIGINT, SIG_IGN);

  signal(SIGUSR1, SIG_IGN);
  signal(I_AM_ALIVE, SIG_IGN);

  /* Creazione della DFsocket */
  int serverFd, clientFd1, clientFd2, clientFd3, serverLen, clientLen;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr *serverSockAddrPtr;
  struct sockaddr_un clientUNIXAddress;
  struct sockaddr *clientSockAddrPtr;
  serverSockAddrPtr = (struct sockaddr *) &serverUNIXAddress;
  serverLen = sizeof(serverUNIXAddress);
  clientSockAddrPtr = (struct sockaddr *) &clientUNIXAddress;
  clientLen = sizeof(clientUNIXAddress);
  serverFd = socket(AF_UNIX, SOCK_STREAM, 0); /* Protocollo di default definito a 0 */
  serverUNIXAddress.sun_family = AF_UNIX;
  strcpy(serverUNIXAddress.sun_path, "DFsocket");
  unlink("DFsocket");
  bind(serverFd, serverSockAddrPtr, serverLen);
  listen(serverFd, 4);

  clientFd1 = accept(serverFd, clientSockAddrPtr, &clientLen);
  clientFd2 = accept(serverFd, clientSockAddrPtr, &clientLen);
  clientFd3 = accept(serverFd, clientSockAddrPtr, &clientLen);
  char result1[6];
  char result2[6];
  char result3[6];
  int conta = 0;

  while (1) {
    /* Recupero i dati */
    read(clientFd1, result1, 6);
    read(clientFd2, result2, 6);
    read(clientFd3, result3, 6);

    fwrite(result1, 1, strlen(result1), voted_output);
    fwrite(", ", 1, 1, voted_output);
    fwrite(result2, 1, strlen(result1), voted_output);
    fwrite(", ", 1, 1, voted_output);
    fwrite(result3, 1, strlen(result1), voted_output);
    fwrite("\n", 1, 1, voted_output);
    fclose(voted_output);

    /* Voto di maggioranza */
    if(strcmp(result1, result2) == 0)
      conta++;
    if(strcmp(result1, result3) == 0)
      conta++;
    if(strcmp(result2, result3) == 0)
      conta++;
    if(conta > 0) {
      fwrite("SUCCESSO\n", 1, 9, system_log);
      fclose(system_log);
      kill(0, I_AM_ALIVE);
    } else {
      fwrite("FALLIMENTO\n", 1, 11, system_log);
      fclose(system_log);
      kill(0,SIGUSR1);
    }
    conta = 0;

    voted_output = fopen(VOTED_OUTPUT, "a");
    system_log = fopen(SYSTEM_LOG, "a");

  }
  /* Chiusure */
  close(clientFd1);
  close(clientFd2);
  close(serverFd);
  unlink("DFsocket");
}
