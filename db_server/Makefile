#Compiler and linker

CC = gcc
LD = gcc

#Relative folder locations
INC_DIR = include
SRC_DIR = src
OBJ_DIR = objects
LIB_DIR = lib
LIBRARYS = lib/librequest.so -lpthread


#Compiler flags
CFLAGS=-I$(INC_DIR) -L$(LIB_DIR) -g

#Get all sources
SRCS = $(wildcard $(SRC_DIR)/*.c)

#Map sources to corresponding object file
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

#Creation of sqlserver
all: sql_server

sql_server: $(OBJS)
	$(LD) $^ -o sql_server -L$(LIB_DIR) $(LIBRARYS)

#Compiling all objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm sql_server $(OBJS)

