CC = gcc
LD = gcc

INC_DIR = include
SRC_DIR = src
OBJ_DIR = objects
LIB_DIR = lib

CFLAGS=-I$(INC_DIR) -L$(LIB_DIR) -g

#Get all sources
SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

all: sql_server

sql_server: $(OBJS)
	$(LD) $^ -o sql_server

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm sql_server $(OBJS)

