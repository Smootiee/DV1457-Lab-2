#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "request.h"
#include "db_manager.h"
#define BUFFER_SIZE 1024

void *clientHandler(void *ptr){
    //If error, pthread_exit, or whole process will crash
    int socket_fd = (int)ptr;
    char* str = malloc(sizeof(char)*BUFFER_SIZE);
    request_t* request;
    char** req_error = malloc(sizeof(char*));
    
    short running = 1;
    
    char *hello = "Hello from server\n";
    send(socket_fd, hello, strlen(hello), 0);

    int read_chars;
    while(running){
        bzero(str,BUFFER_SIZE);
        read_chars = 0;
        read_chars = read(socket_fd, str, BUFFER_SIZE);

        //TODO: check if starts with '.', marking special command
        //read until semicolon
        while(read_chars<BUFFER_SIZE && strchr(str,';') == NULL && str[0] != '.'){
            read_chars += read(socket_fd, &str[read_chars], BUFFER_SIZE-read_chars);
        }

        //TODO: split on semicolon and parse each request seperatly.
        // printf("1%s\n", str);
        request = parse_request(str,req_error);
        // printf("2\n");
        if(request == NULL){
            // printf("3\n");
            send(socket_fd, *req_error, strlen(*req_error), 0);
            free(*req_error);
        }else{
            printf("4\n");
            print_request(request);
            if(request->request_type == RT_QUIT){
                running = 0;
            }else{
                printf("5\n");
                dbRequest(request);
                //TODO: communicate with db
            
            }
            destroy_request(request);
        }
    }
    free(req_error);
    free(str);
    close(socket_fd);
    pthread_exit(1);

    
}