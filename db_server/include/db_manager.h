#ifndef DB_MANAGER_H
#define DB_MANAGER_H
#pragma GCC visibility push(default)

#include "../lib/request.h"

struct metaData{
    char name[255];
    int data_type;
    int is_primary_key;
    int char_size;
    struct metaData* next;
};

struct metaData* getMetaData();

int deleteMetaData(struct metaData* data);

char* dbRequest(request_t* request);

char* createTable(request_t* request);

char* listTables(request_t* request);

char* listSchemas(request_t* request);

char* dropTable(request_t* request);

char* insertRecord(request_t* request);

char* selectStatement(request_t* request);



#pragma GCC visibility pop
#endif