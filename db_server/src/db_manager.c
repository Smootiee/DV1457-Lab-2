#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

#include "../lib/request.h"
#include "db_manager.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_init(lock);




//table_name = actual table
//table_name_data = metadata

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
    pthread_mutex_unlock(&lock);
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
    pthread_mutex_unlock(&lock);
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
        struct metaData* data = getMetaData(path);
        struct metaData* orig = data;
        strcpy(path, "");
        while (data != NULL){
            strcat(path,data->name);
            if(data->data_type == DT_INT){
                strcat(path,"\tINT\n");
            }else{
                char buff[20];
                snprintf(buff,20,"%d", data->char_size);
                strcat(path,"\tVARCHAR(");
                strcat(path, buff);
                strcat(path,")\n");
            }
            data = data->next;
        }
        deleteMetaData(orig);
        fclose(f);
    }
    pthread_mutex_unlock(&lock);
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
    pthread_mutex_unlock(&lock);
    return path;

}

char* insertRecord(request_t* request){
    //check if file exists
    //check if insert is correctly formatted
    //insert into table
    char* path = malloc(sizeof(char)*1500);
    strcpy(path, "../database/");
    strcat(path,request->table_name);
    FILE *ftab = fopen(path, "r");
    if(ftab == NULL){        
        //Table doesn't exists
        strcpy(path,"Table ");
        strcat(path, request->table_name);
        strcat(path," does not exist!\n");
    }else{
        fclose(ftab);
        ftab = fopen(path, "a");
        strcat(path, "_data");
        FILE *fmeta = fopen(path, "r");
        if(fmeta == NULL){
            strcpy(path, "Error opening the table");
            return path;
        }
        struct metaData* data = getMetaData(path);
        struct metaData* orig = data;
        char* entry = malloc(sizeof(char)*1024);
        column_t* currentCol = request->columns;
        char* tempBuff = malloc(sizeof(char)*150);
        int error = 0;
        while(data != NULL && currentCol != NULL && !error){
            if ((data->next == NULL && currentCol->next != NULL) || (data->next != NULL && currentCol->next == NULL)){
                error = 1;
                strcpy(path, "Number of columns not matching\n");
            }
            if(currentCol->data_type != data->data_type){
                strcpy(path, "Datatype mismatch\n");
                error = 1;
            }
            memset(tempBuff,0,150*sizeof(char));
            
            if(data->data_type == DT_INT){
                snprintf(tempBuff,12, "%d ", currentCol->int_val);
            }else if(data->data_type == DT_VARCHAR){
                snprintf(tempBuff,data->char_size+2, "%s ", currentCol->char_val);
            }
            strcat(entry, tempBuff);
            
            data = data->next;
            currentCol = currentCol->next;
        }
        deleteMetaData(orig);

        if(!error){
            strcpy(path, "Inserted values(");
            strcat(path, entry);
            strcat(path,") to table ");
            strcat(path, request->table_name);
            strcat(path, " successfully.\n");
            
            fprintf(ftab,"%s\n", entry);
        }
        

        fclose(ftab);
        fclose(fmeta);
        free(entry);
        free(tempBuff);
    }
    pthread_mutex_unlock(&lock);
    return path;
}

char* selectStatement(request_t* request){
    //check if table exists
    //read and print selected   

    char* path = malloc(sizeof(char)*1024);
    strcpy(path, "../database/");
    strcat(path,request->table_name);
    FILE *f = fopen(path, "r");

    if(f == NULL){
        strcpy(path, "Table doesn't exist\n");
    }else if (request->columns == NULL){
        int size = 1024;
        char line[size];
        int lines =0;
        while (fgets(line,size, f) != NULL){
            lines++;
        }
        if(lines == 0){
            strcpy(path, "--Table is empty--\n");
        }else{
            path = realloc(path,sizeof(char)*1024*lines);
            memset(path,0,sizeof(char)*1024*lines);
            rewind(f);
            while (fgets(line,size, f) != NULL){
                strcat(path, line);
            }
        }
    }
    else
    {
        strcpy(path, "Selecting columns not available\n");
    }
    pthread_mutex_unlock(&lock);
    return path;
}

char* dbRequest(request_t* request){
    char* error;
    pthread_mutex_lock(&lock);
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

struct metaData* getMetaData(char* path){

    FILE *f = fopen(path, "r");
    if (f == NULL){
        return NULL;
    }else{

        struct metaData* data, *current = NULL;
        int size = 255;
        char line[size];

        if(fgets(line,size, f) != NULL){
            data = malloc(sizeof(struct metaData));
            current = data;
            data->next=NULL;
            sscanf(line, "%s %d %d %d\n" ,current->name, &current->data_type, &current->is_primary_key,&current->char_size);
        }
        while (fgets(line,size, f) != NULL){
            current->next = malloc(sizeof(struct metaData));
            current = current->next;
            current->next = NULL;
            sscanf(line, "%s %d %d %d\n" ,current->name, &current->data_type, &current->is_primary_key,&current->char_size);
        }
        
        return data;
    }
}

int deleteMetaData(struct metaData* data){
    if(data->next != NULL){
        deleteMetaData(data->next);
    }
    free(data);
}