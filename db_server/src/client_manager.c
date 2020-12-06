#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "../lib/request.h"
#include "db_manager.h"
#include "client_manager.h"
#define BUFFER_SIZE 1024



volatile sig_atomic_t stopped;

void thread_signal_handler(int signum)
{
    stopped = 1;
}


void *clientHandler(void *ptr){
    stopped = 0;
    struct sigaction sa;
    sa.sa_sigaction = &thread_signal_handler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa,NULL);

    char** req_error;
    char* str;
    int socket_fd;    

    //If error, pthread_exit, or whole process will crash
    socket_fd = (int)ptr;
    str = malloc(sizeof(char)*BUFFER_SIZE);
    request_t* request;
    req_error = malloc(sizeof(char*));
    
    //Hello message sent to client when connecting
    char *hello = "\nHello from server, and welcome to SQLite-shell\n\nAvailable commands:\n.tables\t\t\tLists tables\n.schema <table>\t\tList table schema\n.quit\t\t\tTerminate session\nSELECT * FROM <table>;\nINSERT INTO <table> VALUES (<values);\nDROP TABLE <table>;\nCREATE TABLE <table> (<id> <INT|VARCHAR(<int>)> <comma separated for multiple columns>);\n\n";
    send(socket_fd, hello, strlen(hello), 0);
    char running = 1;
    int read_chars;
    //Main loop for clients. Will Run until either the client requests to disconnect, or the main thread signals that the connection should be closed.
    while(running && !stopped){
        bzero(str,BUFFER_SIZE);
        read_chars = 0;
        read_chars = read(socket_fd, str, BUFFER_SIZE);

        //read until semicolon
        while(read_chars<BUFFER_SIZE && strchr(str,';') == NULL && str[0] != '.' &&!stopped){
            read_chars += read(socket_fd, &str[read_chars], BUFFER_SIZE-read_chars);
        }

        //Parse the incomming request, returns NULL if invalid and stores reason in *req_error
        request = parse_request(str,req_error);
        if(request == NULL){
            //Send the error message to the client  
            send(socket_fd, *req_error, strlen(*req_error), 0);
            send(socket_fd, "\n", 1, 0);
            free(*req_error);
        }else{
            //"log of request to the console", 
            //print_request(request);
            if(request->request_type == RT_QUIT){
                //If the request-type is the quit command, the connection should be closed.
                running = 0;
            }else{  
                //Send the request to the db_manager, which will perform the action, and return the result message. The response needs to be freed by the caller.
                char* response = dbRequest(request);
                //Send response to the client
                send(socket_fd, response, strlen(response), 0);
                free(response);            
            }
            //Caller needs to destroy the request.
            destroy_request(request);
        }
    }
    //free allocated memory.
    free(req_error);
    free(str);
    //Same as main, with the difference being that the thread may exit before the application exits, making it necesary to close the socket to be available for later use
    close(socket_fd);
    //Exit the thread
    pthread_exit(NULL);

    
}