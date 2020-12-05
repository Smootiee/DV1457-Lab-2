#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "../lib/request.h"
#include "client_manager.h"

void printHelpText();

//TODO: exception handler to close threads nicely when ctrl+c

int main(int argc, char const *argv[]){
    //Default port
    int port = 1337;
    //Get commandline arguments
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0){
            printHelpText();
            return 0;
        }
        if(strcmp(argv[i], "-p") == 0){
            i++;
            port = atoi(argv[i]);
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

    //Listen for incoming connections, create thread for each connection
    int nbr_of_threads = 0;
    pthread_t *threads = NULL;
    while(1){
        pthread_t *tmp = realloc(threads, sizeof(pthread_t)*(nbr_of_threads+1));
        if(tmp == NULL){
            perror("Failed to create new client-thread");
            free(threads);
            exit(1);
        }else{
            threads = tmp;
            nbr_of_threads++;
        }

        int new_sock_fd;
        if ((new_sock_fd = accept(socket_fd, (struct sockaddr *)&server_address,(socklen_t*)&server_address)) < 0){
            perror("Failed to accept client");
            exit(EXIT_FAILURE);
        }
        pthread_create(&threads[nbr_of_threads-1],0, clientHandler, (void*)new_sock_fd);
    }
     
    return 0;
    
}

void printHelpText(){
    printf("Lite SQL-server usage\n\n");
    printf("\t-h\t\tPrints this help text\n");
    printf("\t-p <port>\tListen to port number <port> (default:2020)\n");
    printf("\t-d\t\tRun as deamon instead of as a normal program\n");
    printf("\t-l <logfile>\tLog to logfile\n");
    printf("\t-s [fork|thread|prefork|mux]\tSelect request handling method\n");
}