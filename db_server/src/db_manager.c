#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include "request.h"



struct metaData{
    char name[255];
    int data_type;
    int is_primary_key;
    int char_size;
};


//table_name = metadata
//table_name_data = actual table

char* createTable(request_t* request){
    
    char* path = malloc(sizeof(char)*255);
    strcpy(path, "../database/");
    strcat(path,request->table_name);
    FILE *f = fopen(path, "r");
    if(f != NULL){        
        //Table already exists
        strcpy(path,"Table ");
        strcat(path, request->table_name);
        strcat(path," already exists\n");
        fclose(f);
    }else{
        //Create files
        f = fopen(path, "w");
        fclose(f);
        strcat(path, "_data");
        f = fopen(path, "w+");
        //TODO: INSERT METADATA
        //Write down Type, Size and Column.

        column_t *colNams = request->columns;
        

        while (colNams != NULL){
            fprintf(f, "%s %d %d %d\n", colNams->name, colNams->data_type, colNams->is_primary_key, colNams->char_size);  
                 
            colNams = colNams->next;
        }
        
        fclose(f);
        strcpy(path, "Table ");
        strcat(path, request->table_name);
        strcat(path, " created successfully.\n");
    }
    return path;
}

//check files in database_folder and return tablenames
char* listTables(request_t* request){
    
    char* tables = malloc(sizeof(char)*4096);
    strcpy(tables,"");
    DIR* d;
    struct dirent* dir;
    d = opendir("../database");
    if(d != NULL){
        while((dir = readdir(d)) != NULL){
            //Check if file
            if(dir->d_type == 8){
                //points to last _ charachter
                char* under = strrchr(dir->d_name, '_'); 
                //if no underscore, or doesn't end with _data, it's a table name
                if(under == NULL || strcmp(under, "_data") != 0){
                    strcat(tables,dir->d_name);
                    strcat(tables,"\n");
                }
            }           
        }
        closedir(d);
    }
    return tables;
}

char* listSchemas(request_t* request){
    
    //check all metafiles in database folder, print names and info for each.    
    char* path = malloc(sizeof(char)*255);
    strcpy(path, "../database/");
    strcat(path,request->table_name);
    FILE *f = fopen(path, "r");
    if(f == NULL){        
        //Table doesn't exist
        strcpy(path,"Table ");
        strcat(path, request->table_name);
        strcat(path," doesn't exists\n");
        
    }else{
        fclose(f);    
        strcat(path, "_data");
        f = fopen(path, "r");
        strcpy(path,"");
        
        int size = 255;
        char line[size];
        struct metaData data;
        
        while (fgets(line,size, f) != NULL){

            int r = sscanf(line, "%s %d %d %d\n" ,data.name, &data.data_type, &data.is_primary_key,&data.char_size);                        
            // printf("metaData contents:\t%s,%d,%d,%d\n", data.name, data.data_type,data.is_primary_key, data.char_size);
            // fflush(stdout);
            strcat(path,data.name);
            if(data.data_type == DT_INT){
                strcat(path,"\tINT\n");
            }else{
                char buff[20];
                snprintf(buff,20,"%d", data.char_size);
                strcat(path,"\tVARCHAR(");
                strcat(path, buff);
                strcat(path,")\n");
            }
            strcpy(data.name,"");
        }
        fclose(f);
    }
    
    return path;
}

//Check if file exists, and deletes them if so
char* dropTable(request_t* request){
    
    char* path = malloc(sizeof(char)*255);
    strcpy(path, "../database/");
    strcat(path,request->table_name);
    
    FILE* f = fopen(path, "r");
    if(f == NULL){        
        //Table doesn't exist
        strcpy(path,"Table ");
        strcat(path, request->table_name);
        strcat(path," doesn't exist\n");
    }else{
        //delete files
        fclose(f);
        if(!remove(path)){
            strcat(path, "_data");
            remove(path),
            strcpy(path, "Table ");
            strcat(path, request->table_name);
            strcat(path, " dropped succesfully!\n");
        }else{
            strcpy(path, "Failed to drop table ");
            strcat(path, request->table_name);
            strcpy(path,".\n");
        }
    }
    return path;

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
    char* error;
    switch (request->request_type)
    {
        case RT_CREATE:
            return createTable(request);
            break;
        case RT_TABLES:
            return listTables(request);
            break;
        case RT_SCHEMA:
            return listSchemas(request);
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
            error = malloc(sizeof(char)*255);
            strcpy(error, "Request type not defined\n");
            return error;
            
    }
}