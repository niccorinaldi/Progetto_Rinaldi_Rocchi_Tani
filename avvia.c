#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdio.h>

int main(int argc,char* argv[]){
    if(argc<3){
        printf("Inserire tutti gli argomenti necessari\n");
        kill(0,SIGKILL);
    }
    mkdir("bin",0777);
    mkdir("log",0777);
 //   if(fopen("log/alreadyInstalled.txt", "r+") == NULL) {
  //      fopen("log/alreadyInstalled.txt", "w+");
  //      sleep(1);
        system("cc src/InputManager.c -o bin/InputManager && cc src/P1.c -o bin/P1 && cc src/P2.c -o bin/P2 && cc src/P3.c -o bin/P3 && cc src/DecisionFunction.c -o bin/DF && cc src/FailureManager.c -o bin/FM && cc src/Watchdog.c -o bin/WD");
        printf("\nCompilazione completata!\n");
 //   }
    execl("bin/InputManager",argv[0],argv[1], argv[2], NULL);
}
