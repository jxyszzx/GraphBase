#ifndef LPA_H
#define LPA_H

#include "graph.h"

class LPA {
public:
    Graph g;
    VI comms;

    LPA();
    LPA(bool, std::string);
    void algo(int);
    void display();
private:
    bool label();
    void getComms();
};

#endif