#ifndef DB_MANAGER_H
#define DB_MANAGER_H
#pragma GCC visibility push(default)

#include "request.h"

int createTable(request_t* request);

int listTables(request_t* request);

int listChemas(request_t* request);

int dropTable(request_t* request);

int insertRecord(request_t* request);

int select(request_t* request);


#pragma GCC visibility pop
#endif