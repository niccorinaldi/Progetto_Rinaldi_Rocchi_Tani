#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */
#include <unistd.h> //per usare read()

int InputManagerSocketConnection() {
  int clientFd, serverLen, connection; //connection è la differenza tra lui e inputManager -> vedi
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr* serverSockAddrPtr;
  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress; //VEDI BENE
  serverLen = sizeof (serverUNIXAddress); //Vedi bene anche questo (tutte queste 4 righe insomma)
  clientFd = socket (AF_UNIX, SOCK_STREAM, 0); //righe 13 e 14 differenziazione con IM
  serverUNIXAddress.sun_family = AF_UNIX; // dominio del server (client e server sono sulla stessa macchina -> rel.)
  strcpy (serverUNIXAddress.sun_path, "SocketP2"); //nome del server (Nel progetto di Fili è senza virgolette -> vedi)
  do{ //ciclo finché non riesco a stabilire una connessione con il server
      connection = connect (clientFd, serverSockAddrPtr, serverLen); //restituisce 0 in caso di successo, -1 altrimenti
      if(connection == -1){
          printf("InputManager ritenterà la connessione con P2 tra 1 sec\n");
          sleep(1);
      }
  } while (connection == -1);

  return clientFd;
}


int DecisionFunctionSocketConnection()
{
    int clientFd, serverLen, connection;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientFd = socket (AF_UNIX, SOCK_STREAM, 0);
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy (serverUNIXAddress.sun_path, "SOCKETDF");
    do {
        connection = connect (clientFd, serverSockAddrPtr, serverLen);
        if (connection == -1) {
            printf("DecisionFunction ritenterà la connessione con P2 tra 1 sec\n");
            sleep (1);
        }
    }while (connection == -1);

    return clientFd;
}

/* Tratto da cook.c */
int readLine (int fd, char *str) { //quindi scandisco tutti i caratteri, a questo punto devo trovare il modo di sommarli non tenendo in considerazione le virgole
/* Read a single ’\0’-terminated line into str from fd */
int n;
do { /* Read characters until ’\0’ or end-of-input */
  n = read (fd, str, 1); /* Read one character */
 } while (n > 0 && *str++ != '\0');
 return (n > 0);
} /* Return false if end-of-input */

 int sum(char *str) {

//mix
int sumResult = 0; //vedi se puoi non inizializzarlo
  for(int i = strlen(str); i>-2; i--){
    if(str[i] != ',') {
      sumResult += str[i];
    }
  }
}

//M
int random_failure(int sumResult) {
    srand(time(NULL)+20);  // Imposto un valore casuale al seed della funzione random utilizzando time

    // Genero un numero casuale tra 0 e 9, se uguale a 7 modifico result
    int rand = random()%10;
    if(rand == 7)
        sumResult += 20;
    return sumResult;
}

//F
/*int random_failure(int attivo) {
    srand(time(NULL) + 4); // Imposto un valore casuale al seed della funzione random utilizzando time

    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale a 8, allora genera una failure
    if(attivo && (rand()%10) == 8) {
        return 20;
    }
    else return 0;
}*/

//trova alternativa (ripercussione sul main)
void sendToDecisionFunction(int clientDecisionFunction, int sum) {
    int tmp;
    tmp = htonl(sum); // Converte sum in network Byte Order
    write(clientDecisionFunction, &tmp, sizeof(tmp));
}


int main(int argc, char* argv[]) {
  int clientFd;
  int clientDecisionFunction;
  int numChar = atoi(argv[2]);
  char str[numChar]; // 700 perchè le righe sono circa 550 caratteri e sennò va fuori memoria e crasha
  int sumResult = 0;
 //   savePidOnFile("P2", getpid());

 char MODE[15];
 strcpy(MODE, argv[1]);
 int failure = 0;
 if(strcmp(MODE, "FALLIMENTO") == 0) {
     failure = 1;
 }

 clientFd = InputManagerSocketConnection();
  printf("P2 è pronto\n");
  int count = 0;
  while(readLine(clientFd, str)) { // Legge finché trova qualcosa da leggere
    sumResult = sum(str);
    if(failure == 1){
      sumResult += random_failure(sumResult);
    }
    clientDecisionFunction = DecisionFunctionSocketConnection();
    sendToDecisionFunction(clientDecisionFunction, sumResult);
    close(clientDecisionFunction);
    sleep(1);
  }

    clientDecisionFunction = DecisionFunctionSocketConnection();
    sendToDecisionFunction(clientDecisionFunction, -1); // Avviso Decision Function che ho terminato
    close(clientDecisionFunction);
    close (clientFd);
    printf("P2 terminato\n");
    return 0;
}
