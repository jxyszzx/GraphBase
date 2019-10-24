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

scc_tar=scc
wcc_tar=wcc
sssp_tar=sssp
pagerank_tar=pagerank
color_scc_tar = color_scc
graph_info_tar  = graph_info
gc_tar = greedy_color

compile_list = scc wcc sssp pagerank color_scc graph_info greedy_color

all: $(scc_tar) $(wcc_tar) $(sssp_tar) $(pagerank_tar) $(color_scc_tar) $(graph_info_tar)  $(gc_tar)


$(scc_tar) : $(OBJ1) scc.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(wcc_tar) : $(OBJ1) wcc.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(sssp_tar) : $(OBJ1) sssp.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(pagerank_tar) : $(OBJ1) pagerank.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(color_scc_tar) : $(OBJ1) color_scc.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(graph_info_tar) : $(OBJ1) graph_info.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

$(gc_tar) : $(OBJ1) greedy_color.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CXXFLAGS) $(MPFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(CXXFLAGS) $(MPFLAGS) 

clean:
	rm -f *.o *~ $(compile_list)
