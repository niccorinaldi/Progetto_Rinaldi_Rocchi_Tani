#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
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
    result = connect(fdConnessioneSocket, serverSockAddrPtr, serverLen);
    if(result == -1) {
      printf("Problema di connessione P3 -> DecisionFunction, riprovare tra 1 secondo\n");
	    sleep (1);
    }
  } while(result == -1);
  return fdConnessioneSocket;
}

///////////////////////////////////////////////////////////////////
//////// Metodo per l'incremento del risultato ottenuto //////////
/////////////////////////////////////////////////////////////////
int random_failure(int result) {
  srand(time(NULL)+30);

  int rand = random()%10;
  if(rand == 5) /* Probabilità 10^-1 */
    result += 30;
  return result;
}

///////////////////////////////////////////////////////////////////
////////////////////////// Main //////////////////////////////////
/////////////////////////////////////////////////////////////////
void main(int argc, char *argv[]){

  signal(SIGUSR1, SIG_IGN);
  signal(I_AM_ALIVE, SIG_IGN);

  int numChar = atoi(argv[2]) - 1;
  int fallimento = 0;

  /* Verifico se la modalità inserita è FALLIMENTO */
  FILE *fp;
  char MODALITA[15];
  strcpy(MODALITA, argv[1]);
  if(strcmp(MODALITA, "FALLIMENTO") == 0) {
    fallimento = 1;
  }

  /* Creazione di due buffer per verificare quando
  il file condiviso viene aggiornato */
  char buffer[numChar];
  char bufferPrecedente[numChar];
  int result = 0;
  fp = NULL;
  while(fp == NULL) {
    fp = fopen("FileCondivisoP3.txt", "r");
  }
  printf("P3 PRONTO\n");
  fread(buffer, 1, numChar, fp);
  printf("%s\n", buffer);

  strcpy(bufferPrecedente, buffer);

  /* Connessione a DecisionFunction */
  int fdConnessioneSocket = connessioneSocket();

  /* Creazione del buffer di invio */
  char bufferInvio[6];

  while (1){
    if(strcmp(buffer, bufferPrecedente) != 0) { /* Confronto le due stringhe */
      for(int i = 0; i < numChar; i++) {
        if(buffer[i] != ',') {
          result += buffer[i];
        }
      }

      /* Modifica del risultato se avviato in modalità FALLIMENTO */
      if(fallimento == 1)
        result = random_failure(result);

      /* Invio del risultato a DecisionFunction */
      snprintf(bufferInvio, 6, "%d\n", result);
      write(fdConnessioneSocket, bufferInvio, 6);
      result = 0;

      /* Aggiorno bufferPrecedente con il nuovo buffer */
      strcpy(bufferPrecedente, buffer);
    }
    fclose(fp);
    fp = fopen("FileCondivisoP3.txt", "r");
    fread(buffer, 1, numChar, fp);
  }
  /* Chiusure */
  close(fdConnessioneSocket);
}
