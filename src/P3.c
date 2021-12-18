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

int connessioneSocket() {
    int fdConnessioneSocket, serverLen, result;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    fdConnessioneSocket = socket (AF_UNIX, SOCK_STREAM, 0);
    serverUNIXAddress.sun_family = AF_UNIX; /*dominio server*/
    strcpy (serverUNIXAddress.sun_path, "DFsocket"); /*nome server*/

    do { /*itera finchè non viene stabilita la connessione*/
        result = connect (fdConnessioneSocket, serverSockAddrPtr, serverLen);
        if (result == -1) {
	        printf("Problema di connessione P3, riprovare tra 1 secondo\n");
	        sleep (1);
        }
    } while (result == -1);
    return fdConnessioneSocket;
}

int random_failure(int result) {
    /*Imposto un valore casuale al seed della finzione random utilizzando time */
    srand(time(NULL)+30);

    /* Genero un numero casuale tra 0 e 9, se uguale a 1 modifico result */
    int rand = random()%10;
    if(rand == 7)
        result += 30;
    return result;
}

int main(int argc, char *argv[]){

  /* Ignoro i segnali SIGUSR1 e I_AM_ALIVE */
  signal(SIGUSR1, SIG_IGN);
  signal(I_AM_ALIVE, SIG_IGN);

  /* Definisco la lunghezza delle righe da leggere */
  int numChar = atoi(argv[1]) - 1; //converte una stringa in intero

  /*definisco la modalità di avvio*/
  FILE *fp;
  char MODE[15];
  strcpy(MODE, argv[0]);
  int failure=0;
  if(strcmp(MODE, "FALLIMENTO") == 0) {
      failure = 1;
  }

  /* Creazione di due buffer che ospiteranno i dati ricevuti
     da input manager, servono per poter capire quando
     il file condiviso viene aggiornato */
  char buffer[numChar], bufferPrecedente[numChar];
  int result = 0;

  fp = NULL;
  while (fp == NULL){
      fp = fopen("FileCondivisoP3.txt", "r");  /*Apertura del file condiviso*/
  }
  fread(buffer, 1, numChar, fp); //permette di leggere su un file un blocco di dati di qualsiasi tipo
  printf("%s\n", buffer); // %s\n stampa la stringa buffer

  /* Copio buffer in bufferPrecedente in modo da capire se il file è stato aggionato successivamente */
  strcpy(bufferPrecedente, buffer);

  /* Connessione a decisionFunction */
  int fdConnessioneSocket = connessioneSocket();

  /* Creazione del buffer di invio */
  char bufferInvio[6]; //perchè 6?

  while (1) //ciclo infinito, si ferma quando incontra break o return. Perchè 1!=0 è una condizione sempre vera
  {
      /* Controllo che le stringhe siano diverse, altrimenti aspetto una modifica */
      if (strcmp(buffer, bufferPrecedente) != 0)
      {
          for (int i = 0; i < numChar; i++)
          {
              if(buffer[i] != ',') {
                  result += buffer[i];  //effettuo la somma
              }
          }

          /* Modifica del risultato se avviato in modalità FALLIMENTO */
          if(failure == 1)
              result = random_failure(result);

          /* Invio del risultato a decisionFunction */
          snprintf(bufferInvio,6,"%d\n",result); //formatta e memorizza una serie di caratteri e valori nel buffer dell'array
          write(fdConnessioneSocket,bufferInvio,6);
          result = 0;

          /* Aggiorno bufferPrecedente con il nuovo buffer */
          strcpy(bufferPrecedente, buffer);
      }
      fclose(fp);
      fp = fopen("FileCondivisoP3.txt", "r");
      fread(buffer, 1, numChar, fp);
  }
  close(fdConnessioneSocket);

}
