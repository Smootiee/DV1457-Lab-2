#ifndef DB_MANAGER_H
#define DB_MANAGER_H
#pragma GCC visibility push(default)

#include "request.h"

char* dbRequest(request_t* request);

char* createTable(request_t* request);

char* listTables(request_t* request);

char* listChemas(request_t* request);

char* dropTable(request_t* request);

char* insertRecord(request_t* request);

char* selectStatement(request_t* request);


#pragma GCC visibility pop
#endif