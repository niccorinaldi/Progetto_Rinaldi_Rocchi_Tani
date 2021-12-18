#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h> /*For S_IFIFO */
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h> /*For AF_UNIX sockets */
#define I_AM_ALIVE SIGUSR2

int connessioneSocket() {
    int fdConnessioneSocket, serverLen, result;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    fdConnessioneSocket = socket (AF_UNIX, SOCK_STREAM, 0);
    serverUNIXAddress.sun_family = AF_UNIX; /*dominio server*/
    strcpy (serverUNIXAddress.sun_path, "DFsocket");/*nome server*/

    do { /*itera finchè non viene stabilita la connessione*/
        result = connect (fdConnessioneSocket, serverSockAddrPtr, serverLen);
        if (result == -1) {
	        printf("Problema connessione P1; riprovare tra 1 secondo\n");
	        sleep (1);
        }
    } while (result == -1);
    return fdConnessioneSocket;
}

int random_failure(int result) {

    /* Imposto un valore casuale al seed della funzione random utilizzando time */
    srand(time(NULL)+10);

    /* Genero un numero casuale tra 0 e 9, se uguale a 7 modifico result */
    int rand = random()%10;
    if(rand == 5)
        result += 10;
    return result;
}
void main (int argc, char* argv[]) {

    /* Definisco la lunghezza delle righe da leggere */
    int numChar = atoi(argv[2]); //verificare indice di argv (0)
   
    signal(SIGUSR1, SIG_IGN);
    signal(I_AM_ALIVE, SIG_IGN);

    int fdPipe, failure, result = 0;
    char buffer[numChar];

    /*definisco la modalità di avvio*/
    char MODE[15];
    strcpy(MODE, argv[1]);
    if(strcmp(MODE, "FALLIMENTO") == 0) {
        failure = 1;
    }

    /* Rimuozione della pipe con nome se già esiste */
    unlink("PipeP1");

    /* Creazione della pipe con nome */
    mknod ("PipeP1", S_IFIFO, 0);

    /* Settaggio dei permessi */
    chmod ("PipeP1", 0660);
    fdPipe = open ("PipeP1", O_RDONLY);
	
    printf("P1 PRONTO\n");

    /* Connessione a decisionFunction */
    int fdConnessioneSocket = connessioneSocket();

    /* Creazione del buffer di invio */
    char bufferInvio[6];

    /* Ricezione dei messaggi */
    while (read(fdPipe, buffer, numChar);) //verificare quando termina
    {
        //read(fdPipe, buffer, numChar);
        /* Operazione sui dati ricevuti con string token */
        char* blocco = strtok(buffer, ",");
        while (blocco != NULL)
        {
            while (*blocco != '\0' && *blocco != '\n')    /* Scarto i terminatori di riga */
            {
                result += *blocco;
                blocco++;
            }
            blocco = strtok(NULL, ",");
        }

        /* Modifica del risultato se avviato in modalità FALLIMENTO */
        if(failure == 1)
            result = random_failure(result);

        /* Invio del risultato a decisionFunction */
        snprintf(bufferInvio,6,"%d\n",result);/* Salvataggio del risultato in un buffer di invio */
        write(fdConnessioneSocket,bufferInvio,6);
        result = 0;
    }
    close (fdPipe);
    close (fdConnessioneSocket);
    unlink("PipeP1");
    printf("P1 TERMINATO\n");
}
