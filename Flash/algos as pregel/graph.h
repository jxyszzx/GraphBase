#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <iostream>
#include <vector>
#include <omp.h>

typedef std::vector<int> VI;
typedef std::vector<VI> VVI;

class Graph {
public:
    VVI links;
    VI label;

    Graph();
    Graph(bool, int, std::string filename);
    void display();
private:
    void build_undirected_graph(std::ifstream&);
    void build_directed_graph(std::ifstream&);
    bool read(int&);
};

#endif