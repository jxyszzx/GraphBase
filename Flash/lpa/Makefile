#!/bin/bash

CC = g++
CFLAGS = -ansi -O5 -Wall
LDFLAGS = -ansi -lm -Wall
EXEC = main
OBJ1 = graph.o lpa.o
CXXFLAGS:= -std=c++11
MPFLAGS:= -fopenmp

all: $(EXEC)

main : $(OBJ1) main.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS) $(CXXFLAGS) $(MPFLAGS)

clean:
	rm -f *.o *~ $(EXEC)
