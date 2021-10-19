#include <stdio.h>

int main(int argc, char *argv[]){
  FILE *filePointer = fopen(argv[?], "r"); //Apertura file dataset.csv in sola lettura
  char carattereCorrente;
  int numChar=0;

  if(filePointer == NULL){  //controllo che il file si sia aperto correttamente
    printf("Errore apertura file");
    exit(1); //da controllare !!!!
  }

  //Scartare la prima riga

  while(carattereCorrente != "\n"){
    carattereCorrente = fgetc(filePointer); //fgetc scorre un carattere alla volta
  }


  //Trovare la dimensione del Buffer(dimRiga)

  while(carattereCorrente != "\n"){
    carattereCorrente = fgetc(filePointer);
    numChar++;
  }

  char buffer[numChar]; //tutte le righe dovrebbero essere uguali

  //Scorrerre tutto il file inviando le righe ai processi con tempo 1 secondo

  /* Connessione alla pipe di P1*/
  int fdConnessionePipe = connessionePipe();
  int fdFileCondiviso = fileCondiviso();

  while(fgets(buffer, numChar, filePointer)){
    write(fdConnessionePipe, buffer, numChar);
    //p2
    write(fdFileCondiviso, buffer, numChar);
    sleep(1);
  }

  //chiudere tutto (fclose)
}

//metodo per la creazione di P1 --> pipe
int connessionePipe(){
  int fd=0;

  while(fd != -1){
    fd = open("NomeFIle", O_WRONLY) //apertura pipe in sola scrittura
    if(fd == -1){
      printf("Errore apetura pipe");
      fd=0;
      sleep(1);
    }
    else{
      break;
    }
  }
  return fd;
}
//metodo per la creazione di P2

//metodo per la creazione di P3
int fileCondiviso(){
  int fd=0;

  while(fd != -1){
    fd = open("NomeFIle", O_CREAT|O_WRONLY|O_TRUNC, 0777) //creazione file condiviso create-> creare | O_WRONLY-> sola scrittura | O_TRUNC-> se il file esiste gia lo tronca a zero
    if(fd == -1){
      printf("Errore apetura pipe");
      fd=0;
      sleep(1);
    }
    else{
      break;
    }
  }
  return fd;
}
