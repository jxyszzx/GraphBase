#!/bin/bash
CC = g++
CFLAGS = -O2 -Wall
LDFLAGS = -Wall

OBJ1 = graph.o
CXXFLAGS:= -std=c++11
MPFLAGS:= -fopenmp

wcc_tar=wcc
sssp_tar=sssp
pagerank_tar=pagerank

compile_list = wcc sssp pagerank

all: $(wcc_tar) $(sssp_tar) $(pagerank_tar)

$(wcc_tar) : $(OBJ1) wcc.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(sssp_tar) : $(OBJ1) sssp.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(pagerank_tar) : $(OBJ1) pagerank.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS) $(CXXFLAGS) $(MPFLAGS)

clean:
	rm -f *.o *~ $(compile_list)