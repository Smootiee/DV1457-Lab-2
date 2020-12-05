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
    
    char *hello = "\nHello from server, and welcome to SQLite-shell\n\nAvailable commands:\n.tables\t\t\tLists tables\n.schema <table>\t\tList table schema\n.quit\t\t\tTerminate session\nSELECT * FROM <table>;\nINSERT INTO <table> VALUES (<values);\nDROP TABLE <table>;\nCREATE TABLE <table> (<id> <INT|VARCHAR(<int>) <comma separated for multiple columns>);\n\n";
    send(socket_fd, hello, strlen(hello), 0);
    char running = 1;
    int read_chars;
    while(running && !stopped){
        bzero(str,BUFFER_SIZE);
        read_chars = 0;
        read_chars = read(socket_fd, str, BUFFER_SIZE);

        //read until semicolon
        while(read_chars<BUFFER_SIZE && strchr(str,';') == NULL && str[0] != '.' &&!stopped){
            read_chars += read(socket_fd, &str[read_chars], BUFFER_SIZE-read_chars);
        }

        //TODO: split on semicolon and parse each request seperatly.
        request = parse_request(str,req_error);
        if(request == NULL){  
            send(socket_fd, *req_error, strlen(*req_error), 0);
            send(socket_fd, "\n", 1, 0);
            free(*req_error);
        }else{
            print_request(request);
            if(request->request_type == RT_QUIT){
                running = 0;
            }else{  
                char* response = dbRequest(request);
                send(socket_fd, response, strlen(response), 0);
                free(response);            
            }
            destroy_request(request);
        }
    }
    free(req_error);
    free(str);
    
    close(socket_fd);
    pthread_exit(NULL);

    
}