#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define I_AM_ALIVE SIGUSR2

///////////////////////////////////////////////////////////////////
/// Metodo per la connessione a DecisionFunction tramite socket //
/////////////////////////////////////////////////////////////////
int connessioneSocket() {
  int fdConnessioneSocket, serverLen, result;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr* serverSockAddrPtr;
  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  serverLen = sizeof(serverUNIXAddress);
  fdConnessioneSocket = socket(AF_UNIX, SOCK_STREAM, 0); /* Protocollo di default definito a 0 */
  serverUNIXAddress.sun_family = AF_UNIX;
  strcpy (serverUNIXAddress.sun_path, "DFsocket");

  do {
      result = connect (fdConnessioneSocket, serverSockAddrPtr, serverLen);
      if (result == -1) {
	       printf("Problema di connessione P1 -> DecisionFunction, riprovare tra 1 secondo\n");
	       sleep (1);
      }
  } while (result == -1);
  return fdConnessioneSocket;
}

///////////////////////////////////////////////////////////////////
//////// Metodo per l'incremento del risultato ottenuto //////////
/////////////////////////////////////////////////////////////////
int random_failure(int result) {
  srand(time(NULL)+10);

  int rand = random()%10;
  if(rand == 5) /* Probabilità 10^-1 */
    result += 10;
  return result;
}

///////////////////////////////////////////////////////////////////
////////////////////////// Main //////////////////////////////////
/////////////////////////////////////////////////////////////////
void main (int argc, char* argv[]) {

  int numChar = atoi(argv[2]);

  signal(SIGUSR1, SIG_IGN);
  signal(I_AM_ALIVE, SIG_IGN);

  int fdPipe = 0;
  int fallimento = 0;
  int result = 0;
  char buffer[numChar];

  /* Verifico se la modalità inserita è FALLIMENTO */
  char MODALITA[15];
  strcpy(MODALITA, argv[1]);
  if(strcmp(MODALITA, "FALLIMENTO") == 0) {
      fallimento = 1;
  }

  /* Rimuozione della pipe se già esistente */
  unlink("PipeP1");

  /* Creazione della pipe */
  mknod ("PipeP1", S_IFIFO, 0);

  chmod ("PipeP1", 0660);
  fdPipe = open ("PipeP1", O_RDONLY);

  printf("P1 PRONTO\n");

  /* Connessione a DecisionFunction */
  int fdConnessioneSocket = connessioneSocket();

  /* Creazione del buffer di invio */
  char bufferInvio[6];

  while (1) {
    read(fdPipe, buffer, numChar);
    char* blocco = strtok(buffer, ",");
    while(blocco != NULL) {
      while(*blocco != '\0' && *blocco != '\n') { /* Scarto i terminatori di riga */
        result += *blocco;
        blocco++;
      }
      blocco = strtok(NULL, ",");
    }

    /* Modifica del risultato se avviato in modalità FALLIMENTO */
    if(fallimento == 1)
        result = random_failure(result);

    /* Invio del risultato a DecisionFunction */
    snprintf(bufferInvio, 6, "%d\n", result);
    write(fdConnessioneSocket, bufferInvio, 6);
    result = 0;
  }
  /* Chiusure */
  close (fdPipe);
  close (fdConnessioneSocket);
}
