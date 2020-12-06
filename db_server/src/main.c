#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>

#include "../lib/request.h"
#include "client_manager.h"

void printHelpText();

volatile sig_atomic_t exitRequested = 0;

//When ctrl+c is pressed, main loop will stop. Also, it's likely that this thread is stuck at the accept() call, but accept() call will return with error
//telling us that an interupt has occured, making it easy to cleanup.
void signal_handler(int signum)
{
    exitRequested = 1;
}


int main(int argc, char const *argv[]){
    //setup of signal handler, to perform cleanup after ctrl+c is pressed(SIGINT)
    struct sigaction sa;
    sa.sa_sigaction = &signal_handler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sa,NULL);

    //Default port
    int port = 20000;
    //Get commandline arguments
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0){
            printHelpText();
            return 0;
        }
        else if(strcmp(argv[i], "-p") == 0){
            i++;
            port = atoi(argv[i]);
        }
        else if(strcmp(argv[i], "-s") == 0){
            i++;
            if(strcmp(argv[i], "thread") != 0){
                printf("Request handling method %s not implemented\n", argv[i]);
                printHelpText();
                exit(3);
            }
        }        
        else {
            printf("%s not implemented\n", argv[i]);
            printHelpText();
            exit(3);
        }
    }
    
    
    //Create the listening socket
    int socket_fd;
    struct sockaddr_in server_address;
    //IPv4, TCP
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror("Creation of server socket failed");
        exit(1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    //Binding socket to port
    if(bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address))<0){
        perror("Error on binding socket");
        exit(1);
    }
    printf("Socket successfully bound to port %d\n", port);
    listen(socket_fd,5);

    //Main loop listening for incoming connections, create thread for each connection
    int nbr_of_threads = 0;
    pthread_t *threads = NULL;
    while(!exitRequested){
        pthread_t *tmp = realloc(threads, sizeof(pthread_t)*(nbr_of_threads+1));
        if(tmp == NULL){
            perror("Failed to create new client-thread");
            free(threads);
            exit(1);
        }else{
            threads = tmp;
            nbr_of_threads++;
        }

        int new_sock_fd = accept(socket_fd, (struct sockaddr *)&server_address,(socklen_t*)&server_address);
        if ((new_sock_fd) < 0){
            //Will occur if accept is interupted by signal, and then we want to terminate properly.
            if(errno=EINTR){
                continue;
            }
            perror("Failed to accept client");
            exit(EXIT_FAILURE);
        }else{
            pthread_create(&threads[nbr_of_threads-1],0, clientHandler, (void*)new_sock_fd);
        }
        
    }

    //No longer listening for incomming connections
    for(int i =0; i < nbr_of_threads-1; i++){
        void **ptr;
        //sending user signal 1, resulting in termination of connection and closure of connection by the client threads.
        //This is the only time our threads need to communicate with eachother.
        pthread_kill(threads[i], SIGUSR1);
        //wait for threads to succedd with cleanup
        pthread_join(threads[i],ptr);
    }

    //Closing the main socket
    /*According to the almighty stackoverflow, this is not necesary due to the socket being freed by the OS when terminated, 
    and will not free the socket immediately. This is the way sockets are implemented, 
    and the ways available to circumvent this shortcomming is not truly working, and are condidered
    hacks and not best practice. So in our implementation we have skipped the "hacks" to free the port
    immediately, but still close the socket manually. 
    */
    close(socket_fd);   
    free(threads);
    return 0;
    
}

void printHelpText(){
    printf("Lite SQL-server usage\n\n");
    printf("\t-h\t\tPrints this help text\n");
    printf("\t-p <port>\tListen to port number <port> (default:20000)\n");
    printf("\t-d[x]\t\tRun as deamon instead of as a normal program \n");
    printf("\t-l[x] <logfile>\tLog to logfile\n");
    printf("\t-s [fork[x]|thread|prefork[x]|mux[x]]\tSelect request handling method\n");
    printf("\nEntries marked with [x] are not implemented\n");
}