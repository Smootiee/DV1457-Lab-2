#!/bin/bash

make
export LD_LIBRARY_PATH=lib
./sql_server
