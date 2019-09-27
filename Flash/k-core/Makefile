#!/bin/bash
CC = g++
CFLAGS = -O2 -Wall
LDFLAGS = -Wall

OBJ1 = graph.o
CXXFLAGS:= -std=c++11
MPFLAGS:= -fopenmp

core_tar=core

compile_list = core

all: $(core_tar)

$(core_tar) : $(OBJ1) core.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS) $(CXXFLAGS) $(MPFLAGS)

clean:
	rm -f *.o *~ $(compile_list)
