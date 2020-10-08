#include "request.h"



int createTable(request_t* request){
    //check if table exists
    //If it does, return
    //If it doesn't, create it
}

int listTables(request_t* request){
    //check files in database_folder and print    
}

int listChemas(request_t* request){
    //check all metafiles in database folder, print names and info for each.    
}

int dropTable(request_t* request){
    //Check if file exists, and deletes them if so    
}

int insertRecord(request_t* request){
    //check if file exists
    //check if insert is correctly formatted
    //insert into table    
}

int select(request_t* request){
    //check if table exists
    //read and print selected where   
}