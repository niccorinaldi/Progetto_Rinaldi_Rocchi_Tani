#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define I_AM_ALIVE SIGUSR2

int flag = 0;

void aliveControl(int sig) {
  flag = 0;
}

///////////////////////////////////////////////////////////////////
////////////////////////// Main //////////////////////////////////
/////////////////////////////////////////////////////////////////
void main() {
  signal(I_AM_ALIVE, aliveControl);

  while (1) {
    sleep(1);
    if(flag == 2)
      kill(0, SIGUSR1);
    flag++;
  }
}
