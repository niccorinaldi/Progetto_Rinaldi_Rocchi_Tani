#include <stdio.h> // controllare le librerie che servono
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

//metodo per la creazione di P1 --> pipe
int connessionePipe(){
  int fdPipe=0;

  while(fdPipe != -1){
    fdPipe = open("NomeFIle", O_WRONLY); //apertura pipe in sola scrittura
    if(fdPipe == -1){
      printf("Errore apetura pipe");
      fdPipe = 0;
      sleep(1);
    }
    else{
      break;
    }
  }
  return fdPipe;
}

//metodo per comunicare con P2 tramite socket
int socketConnection() {

  int socketFd, clientFd, serverLen, clientLen;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr* clientSockAddrPtr;
  struct sockaddr* serverSockAddrPtr;
  struct sockaddr_un clientUNIXAddress;

  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress; //VEDI BENE
  serverLen = sizeof (serverUNIXAddress); //Vedi bene anche questo (tutte queste 4 righe insomma)
  clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
  clientLen = sizeof (clientUNIXAddress);

  socketFd = socket (AF_UNIX, SOCK_STREAM, 0); // istanzia la socket  //metti anche 0 invece di "DEFAULT_PROTOCOL"  //file descriptor (descrittore di file associato al nuovo socket creato)
  serverUNIXAddress.sun_family = AF_UNIX; // dominio del server (client e server sono sulla stessa macchina -> rel.)
  strcpy (serverUNIXAddress.sun_path, "SocketP2");
  unlink ("SocketP2"); //invochiamo un unlink() del nome "SocketP2" perché potrebbe già esistere un sockey avente lo stesso nome (causando errore)
  bind(socketFd, serverSockAddrPtr, serverLen); // definisce il nome della socket //VEDI BENE
  listen(socketFd, 1);  //ascolta la coda delle connessioni  //In questo modo gli dico che il numero massimo di richieste pendenti è 1

  clientFd = accept(socketFd, clientSockAddrPtr, &clientLen); // gestisce la prossima connessione  //clientFd = nuovo descrittore di file
  //sfd == Socket File Descriptor
  return clientFd;
}

//metodo per la creazione di P3
int fileCondiviso(){
  int fdFile = 0;

  while(fdFile != -1){
    fdFile = open("NomeFIle", O_CREAT|O_WRONLY|O_TRUNC, 0777); //creazione file condiviso create-> creare | O_WRONLY-> sola scrittura | O_TRUNC-> se il file esiste gia lo tronca a zero
    if(fdFile == -1){
      printf("Errore apetura pipe");
      fdFile=0;
      sleep(1);
    }
    else{
      break;
    }
  }
  return fdFile;
}

int main(int argc, char *argv[]){

  //Apertura file dataset.csv in sola lettura
  FILE *filePointer = fopen(argv[2], "r"); // ???
  if(filePointer == NULL){  //controllo che il file si sia aperto correttamente
    perror("Errore apertura file");  //si può usare anche printf
    exit(1);   // sleep(1); //segnala l'interruzione anomala del programma (EXIT_FAILURE)
  }

  char carattereCorrente;
  int numChar=0;

  /* Settaggio della modalità */
  char MODE[15]; //crea un array di 15 con nome MODE
  strcpy(MODE, argv[1]); //copia l'elemento in pos. 0 di argv e lo mette in MODE
  if(strcmp(MODE,"NORMALE")==0 || strcmp(MODE,"FALLIMENTO")==0){  //strcmp confronta due stringhe (restituisce 0 se sono uguali): MODE con NORMALE e MODE con FALLIMENTO
      printf("MODALITA': %s\n", MODE);
  }else{
      printf("Inserire una modalità valida: FALLIMENTO o NORMALE\n");
      kill(0,SIGKILL); //invia il segnale SIGKILL(terminazione immediata del processo) a ogni process group del processo
  }

  //Scartare la prima riga
  while(carattereCorrente != '\n'){
    carattereCorrente = fgetc(filePointer); //fgetc scorre un carattere alla volta
  }

  //Trovare la dimensione del Buffer(dimRiga)
  while(carattereCorrente != '\n'){
    carattereCorrente = fgetc(filePointer);
    numChar++;
  }

  fseek(filePointer, -numChar, 1); // Riposizionamento all'inizio della seconda riga
  char buffer[numChar]; //tutte le righe dovrebbero essere uguali

  //Scorrerre tutto il file inviando le righe ai processi con tempo 1 secondo

  /* Connessione alla pipe di P1*/
  int fdConnessionePipe = connessionePipe();
  /* Connessione alla socket di P2*/
  int fdConnessioneSocket = socketConnection();
  /*Connessione al file condiviso di P3*/
  int fdFileCondiviso = fileCondiviso();

  while(fgets(buffer, numChar, filePointer)){
    write(fdConnessionePipe, buffer, numChar);
    write (fdConnessioneSocket, buffer, numChar); //sendToSocket
    write(fdFileCondiviso, buffer, numChar);
    sleep(1);
  }

  //chiudere tutto (fclose)
  close(fdConnessionePipe); //chiusura client
  close(fdConnessioneSocket); //chiusura socket
  close(fdFileCondiviso);
  printf("Finito :) \n");
  return 0;
}
