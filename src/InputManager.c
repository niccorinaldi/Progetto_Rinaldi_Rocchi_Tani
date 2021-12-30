#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#define I_AM_ALIVE SIGUSR2

//////////////////////////////////////////////////////////
/////// Metodo per la connessione a P1 tramite pipe /////
////////////////////////////////////////////////////////
int connessionePipe() {
  int fdPipe = 0;
  while(fdPipe != -1){
    fdPipe = open("PipeP1", O_WRONLY); /* Apertura della pipe in sola scrittura */
    if(fdPipe == -1){
      printf("Errore apetura pipe\n");
      fdPipe = 0;
      sleep(1);
    } else {
        break;
    }
  }
  return fdPipe;
}

//////////////////////////////////////////////////////////
/////// Metodo per la connessione a P2 tramite socket ///
////////////////////////////////////////////////////////
int connessioneSocket() {
  int fdConnessioneSocket, serverLen, result;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr* serverSockAddrPtr;
  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  serverLen = sizeof(serverUNIXAddress);
  fdConnessioneSocket = socket(AF_UNIX, SOCK_STREAM, 0); /* Protocollo di default definito a 0 */
  serverUNIXAddress.sun_family = AF_UNIX;
  strcpy(serverUNIXAddress.sun_path, "SocketP2");
  do {
    result = connect(fdConnessioneSocket, serverSockAddrPtr, serverLen);
 		if(result == -1) {
  		printf("Problema di connessione, riprovare tra 1 sec\n");
  		sleep(1);
  	}
  } while(result == -1);
  return fdConnessioneSocket;
}

//////////////////////////////////////////////////////////
// Metodo per la connessione a P3 tramite file condiviso/
////////////////////////////////////////////////////////
int fileCondiviso() {
  int fdFileCondiviso = 0;
  while(fdFileCondiviso != -1) {
    fdFileCondiviso = open("FileCondivisoP3.txt", O_CREAT|O_WRONLY|O_TRUNC, 0777);
    if(fdFileCondiviso == -1) {
      printf("Errore apetura file\n");
      fdFileCondiviso = 0;
      sleep(1);
    } else {
        break;
    }
  }
  return fdFileCondiviso;
}

//////////////////////////////////////////////////////////
////////// Metodo per la creazione delle fork ///////////
////////////////////////////////////////////////////////
void creaFork(char MODALITA[15], char numChar[4]) {
  int pid;

  pid = fork();    /*creazione di DecisionFunction*/
  if(pid < 0) {
    fprintf(stderr, "Fork fallita\n");
    exit(-1);
  } else if(pid == 0) {
      execl("bin/DecisionFunction", "bin/DecisionFunction", NULL);
  }

  pid = fork();   /*Creazione P1*/
  if(pid < 0) {
    fprintf(stderr, "Fork fallita\n");
    exit(-1);
  } else if (pid == 0) {
      execl("bin/P1", "bin/P1", MODALITA, numChar, NULL);
  }

  pid = fork();   /*Creazione di P2*/
  if(pid<0) {
    fprintf(stderr, "Fork fallita\n");
    exit(-1);
  } else if(pid == 0) {
      execl("bin/P2", "bin/p2", MODALITA, numChar, NULL);
  }

  pid = fork();   /*Creazione di P3*/
  if(pid < 0) {
    fprintf(stderr, "Fork fallita\n");
    exit(-1);
  } else if(pid == 0) {
      execl("bin/P3", "bin/P3", MODALITA, numChar, NULL);
  }

  pid = fork();   /*Creazione di FailureManager*/
  if(pid < 0) {
    fprintf(stderr, "Fork fallita\n");
    exit(-1);
  } else if(pid == 0) {
      execl("bin/FailureManager", "bin/FailureManager", NULL);
  }

  pid = fork();   /*Creazione di Watchdog*/
  if(pid < 0) {
    fprintf(stderr, "Fork fallita\n");
    exit(-1);
  } else if(pid == 0) {
      execl("bin/Watchdog", "bin/Watchdog", NULL);
  }
}

//////////////////////////////////////////////////////////
////////////////////// Main /////////////////////////////
////////////////////////////////////////////////////////
int main(int argc, char *argv[]){

  signal(SIGUSR1, SIG_IGN);
  signal(I_AM_ALIVE, SIG_IGN);

  /*Apertura file Dataset.csv in sola lettura*/
  FILE *filePointer = fopen(argv[2], "r");
  if(filePointer == NULL) {
    perror("Errore apertura file dataset\n");
    exit(1);
  }

  char carattereCorrente;
  int numChar = 0;

  /* Verifica della modalità inserita */
  char MODALITA[15];
  strcpy(MODALITA, argv[1]);
  if(strcmp(MODALITA,"NORMALE") == 0 || strcmp(MODALITA,"FALLIMENTO") == 0) {
    printf("MODALITÀ: %s\n", MODALITA);
  } else {
    printf("Inserire una modalità valida: FALLIMENTO o NORMALE\n");
    kill(0,SIGKILL);
  }

  /*Scartare la prima riga*/
  while(carattereCorrente != '\n'){
    carattereCorrente = fgetc(filePointer);
  }

  carattereCorrente++;

  /*Trovare la dimensione del Buffer*/
  while(carattereCorrente != '\n'){
    carattereCorrente = fgetc(filePointer);
    numChar++;
  }

  fseek(filePointer, -numChar, 1); /* Riposizionamento all'inizio della seconda riga*/
  char buffer[numChar]; /*Creazione del buffer*/

  printf("Esecuzione...\n");

  /* File condiviso di P3*/
  FILE* fileCondiviso = fopen("FileCondivisoP3.txt", "w");

  /* Buffer per il passaggio di numChar ai figli,
  scorre il file inviando le righe ai processi con tempo 1 secondo */
  char bufferNumChar[4];
  snprintf(bufferNumChar,4,"%d\n",numChar);

  /* Creazione delle fork */
  creaFork(MODALITA, bufferNumChar);

  /* Connessione alla pipe di P1*/
  int fdConnessionePipe = connessionePipe();
  /* Connessione alla socket di P2*/
  int fdConnessioneSocket = connessioneSocket();

  /*Passaggio di una riga ai processi */
  while(1) {
    fgets(buffer, numChar, filePointer);
    if(*buffer != '\n'){
      write(fdConnessionePipe, buffer, numChar);
      write (fdConnessioneSocket, buffer, numChar);
      fputs(buffer, fileCondiviso);
      fseek(fileCondiviso, 0, SEEK_SET);
      sleep(1); /* Attesa di un secondo */
    }
  }
  /* Chiusure */
  close(fdConnessionePipe);
  close(fdConnessioneSocket);
  fflush(fileCondiviso);
  fclose(fileCondiviso);
  printf("Finito\n");
  return 0;
}
