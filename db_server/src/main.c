
#include "request.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 2020

void printHelpText();


int main(int argc, char const *argv[]){
    //Default port
    int port = 2020;
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
    while(1){
        int new_sock_fd;

        if ((new_sock_fd = accept(socket_fd, (struct sockaddr *)&server_address,(socklen_t*)&server_address)) < 0){
            perror("Failed to accept client");
            exit(EXIT_FAILURE);
        }
    }
    
    
    return 0;



    /*
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char str[1024] = {0};
    char *hello = "Hello from server";
    printHelp();

    // Creating socker file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("Setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0){
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
        perror("Accept");
        exit(EXIT_FAILURE);
    }

    while(1){
    valread = read(new_socket, str, 1024);
    //printf("%s\n", str);
    char** error;
    
    print_request(parse_request(str,error));

    send(new_socket, hello, strlen(hello), 0);
    //printf("Hello message sent\n");
    }
*/
    
}

void printHelpText(){
    printf("Lite SQL-server usage\n\n");
    printf("\t-h\t\tPrints this help text\n");
    printf("\t-p <port>\tListen to port number <port> (default:2020)\n");
    printf("\t-d\t\tRun as deamon instead of as a normal program\n");
    printf("\t-l <logfile>\tLog to logfile\n");
    printf("\t-s [fork|thread|prefork|mux]\tSelect request handling method\n");
}