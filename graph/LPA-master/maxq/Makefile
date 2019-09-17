#!/bin/bash

CC = g++
CFLAGS = -ansi -O5 -Wall
LDFLAGS = -ansi -lm -Wall
EXEC = lpa
OBJ1 = graph.o lpa.o
CXXFLAGS:= -std=c++11

all: $(EXEC)

lpa : $(OBJ1) main.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)


%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS) $(CXXFLAGS)

clean:
	rm -f *.o *~ $(EXEC)
