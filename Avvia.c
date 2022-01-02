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
  usleep(50000);
  system("make install");
  usleep(50000);
  printf("Compilazione terminata!\n\n");
  printf("----------------------------\n AVVIO\n ----------------------------");
  execl("bin/InputManager",argv[0],argv[1], argv[2], NULL);
}
