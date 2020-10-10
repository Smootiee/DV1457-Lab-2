#include <stdlib.h>
#include <stdio.h>

#include "request.h"




char* createTable(request_t* request){
    //check if table exists
    //If it does, return
    //If it doesn't, create it
}

char* listTables(request_t* request){
    //check files in database_folder and print    
}

char* listChemas(request_t* request){
    //check all metafiles in database folder, print names and info for each.    
}

char* dropTable(request_t* request){
    //Check if file exists, and deletes them if so    
}

char* insertRecord(request_t* request){
    //check if file exists
    //check if insert is correctly formatted
    //insert into table    
}

char* selectStatement(request_t* request){
    //check if table exists
    //read and print selected where   
}

char* dbRequest(request_t* request){
    switch (request->request_type)
    {
    case RT_CREATE:
        return createTable(request);
        break;
    case RT_TABLES:
        return listTables(request);
        break;
    case RT_SCHEMA:
        return listChemas(request);
        break;
    case RT_DROP:
        return dropTable(request);
        break;
    case RT_INSERT:
        return insertRecord(request);
        break;
    case RT_SELECT:
        return selectStatement(request);
        break;    
    default:
        printf("REQUEST NOT DEFINED");
        exit(1);
    }
}