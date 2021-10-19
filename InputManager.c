#include <stdio.h>

int main(int argc, char *argv[]){
  FILE *filePointer = fopen(argv[?], "r"); //Apertura file dataset.csv in sola lettura
  char carattereCorrente;
  int numChar=0;

  if(filePointer == NULL){  //controllo che il file si sia aperto correttamente
    printf("Error opening file");
    exit(1); //da controllare !!!!
  }

  //Scartare la prima riga

  while(carattereCorrente != "\n"){
    carattereCorrente = fgetc(filePointer);
  }


  //Trovare la dimensione del Buffer(dimRiga)

  while(carattereCorrente != "\n"){
    carattereCorrente = fgetc(filePointer);
    numChar++;
  }

  char buffer[numChar];

  //Scorrerre tutto il file inviando le righe ai processi con tempo 1 secondo

  while(fgets(buffer, numChar, filePointer)){
    //p1
    //p2
    //p3
    sleep(1);
  }

  //chiudere tutto (fclose)
}

//metodo per la creazione di P1

//metodo per la creazione di P2

//metodo per la creazione di P3
