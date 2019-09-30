#!/bin/bash
CC = g++
CFLAGS = -O2 -Wall
LDFLAGS = -Wall

OBJ1 = graph.o
CXXFLAGS:= -std=c++11
MPFLAGS:= -fopenmp

bc_tar=bc

compile_list = bc

all: $(bc_tar)

$(bc_tar) : $(OBJ1) betweenness.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS) $(CXXFLAGS) $(MPFLAGS)

clean:
	rm -f *.o *~ $(compile_list)
