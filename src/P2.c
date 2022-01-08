#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#define I_AM_ALIVE SIGUSR2

///////////////////////////////////////////////////////////////////
////// Metodo per la ricezione da InputManager tramite socket ////
/////////////////////////////////////////////////////////////////
int inputManagerSocketConnection() {
  int serverFd, clientFd, serverLen, clientLen;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr* serverSockAddrPtr;
  struct sockaddr_un clientUNIXAddress;
  struct sockaddr* clientSockAddrPtr;
  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  serverLen = sizeof(serverUNIXAddress);
  clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
  clientLen = sizeof(clientUNIXAddress);
  serverFd = socket(AF_UNIX, SOCK_STREAM, 0); /* Protocollo di default definito a 0 */
  serverUNIXAddress.sun_family = AF_UNIX;
  strcpy(serverUNIXAddress.sun_path, "SocketP2");
  unlink("SocketP2");
  bind(serverFd, serverSockAddrPtr, serverLen);
  listen(serverFd, 5);
  clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);

  return clientFd;
}

///////////////////////////////////////////////////////////////////
/// Metodo per la connessione a DecisionFunction tramite socket //
/////////////////////////////////////////////////////////////////
int decisionFunctionSocketConnection() {
  int clientFd, serverLen, result;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr* serverSockAddrPtr;
  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  serverLen = sizeof(serverUNIXAddress);
  clientFd = socket(AF_UNIX, SOCK_STREAM, 0); /* Protocollo di default definito a 0 */
  serverUNIXAddress.sun_family = AF_UNIX;
  strcpy(serverUNIXAddress.sun_path, "DFsocket");
  do {
      result = connect (clientFd, serverSockAddrPtr, serverLen);
      if (result == -1) {
        printf("Problema di connessione P2 -> DecisionFunction, riprovare tra 1 secondo\n");
        sleep (1);
      }
  } while(result == -1);

  return clientFd;
}

///////////////////////////////////////////////////////////////////
//////// Metodo per l'incremento del risultato ottenuto //////////
/////////////////////////////////////////////////////////////////
int random_failure(int result) {
  srand(time(NULL)+20);

  int rand = random()%10;
  if(rand == 5) /* Probabilità 10^-1 */
      result += 20;
  return result;
}

///////////////////////////////////////////////////////////////////
////////////////////////// Main //////////////////////////////////
/////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {

  signal(SIGUSR1, SIG_IGN);
  signal(I_AM_ALIVE, SIG_IGN);

  int numChar = atoi(argv[2]);
  char buffer[numChar];
  int fallimento = 0;

  /* Verifico se la modalità inserita è FALLIMENTO */
  char MODALITA[15];
  strcpy(MODALITA, argv[1]);
  if(strcmp(MODALITA, "FALLIMENTO") == 0) {
    fallimento = 1;
  }

  /* Creazione di SocketP2 */
  int clientFd = inputManagerSocketConnection();

  printf("P2 PRONTO\n");

  char bufferInvio[6];
  int result = 0;

  /* Connessione a DecisionFunction */
  int fdConnessioneSocket = decisionFunctionSocketConnection();

  while(1) {
    read(clientFd, buffer, numChar);

    for(int i = strlen(buffer); i>-2; i--) {
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

    sleep(1);
  }
  /* Chiusure */
  close(clientFd);
  close(fdConnessioneSocket);
  return 0;
}
