#!/bin/bash
# Compile Macro
CC = g++
CFLAGS = -Ofast -Wall
LDFLAGS = -Wall

# Dir
# DIR_INC = ../include
# DIR_SRC = ../src

OBJ1 = ../include/graph.o
CXXFLAGS:= -std=c++11
MPFLAGS:= -fopenmp

core_tar=core_parallel

compile_list = core_parallel

all: $(core_tar)

$(core_tar) : $(OBJ1) core_parallel.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(CXXFLAGS) $(MPFLAGS) 

clean:
	rm -f *.o *~ $(compile_list)
