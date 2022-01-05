/*
AUTORI DEL PROGETTO:
Niccolò Rinaldi
Ilaria Rocchi
Chiara Tani
MODULO DI LABORATORIO DI SISTEMI OPERATIVI A.A. 2020/2021
*/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdio.h>

//////////////////////////////////////////////////////////
////////////////////// Main /////////////////////////////
////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
  if(argc<3){
    printf("Inserire tutti gli argomenti necessari\n");
    kill(0,SIGKILL);
  }
  system("make clean");
  system("make");
  system("make install");
  printf("\n\nCompilazione terminata!\n\n");
  printf("----------------------------\n");
  execl("bin/InputManager",argv[0],argv[1], argv[2], NULL);
}
