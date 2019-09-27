#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <queue>
#include <set>
#include <omp.h>
#include "graph.h"
using namespace std;

// #define DEBUG

typedef vector<int> VI;
typedef vector<VI> VVI;

const int maxn = 1e5+5;

vector<long double> msgs;
vector<long double> pagerank;

Graph g;

long double change;
int max_iter;
long double damping_factor, min_delta, damping_value;

void do_page_rank() {
    vector<long double> new_pagerank(g.label.size(), damping_value);
    #pragma omp parallel for
    for (int i = 0; i < g.links.size(); ++i) {
        const VI& nbrs = g.links[i];
        long double tmp = damping_factor*pagerank[i]/nbrs.size();
        for (int j = 0; j < nbrs.size(); ++j) {
            #pragma omp critical
            new_pagerank[nbrs[j]] += tmp;
        }
    }
    #pragma omp parallel for
    for (int i = 0; i < pagerank.size(); ++i) {
        #pragma omp critical
            change += fabs(new_pagerank[i] - pagerank[i]);
        pagerank[i] = new_pagerank[i];
    }
}

void printPR() {
    set<int> comms;
    for (auto label: g.label) {
        comms.insert(label);
    }

    printf("pagerank := { ");
    for (auto pr: pagerank) {
        printf("[%Lf] ", pr);
    }
    puts(" }");
}

int main(int argc, char *argv[])
{
    string filename(argv[1]);
    filename = "../datasets/" + filename;
    g = Graph(true, filename);
    // Init
    max_iter = 1000;
    min_delta = 1e-7;
    damping_factor = 0.85;
    damping_value = (1-damping_factor)/g.label.size();
    pagerank.resize(g.label.size(), 1.0/g.label.size());
    // deal with those nodes with no out-degree
    {
        VI tmp;
        for (int i = 0; i < g.links.size(); ++i) {
            tmp.push_back(i);
        }
        #pragma omp parallel for
        for (int i = 0; i < g.links.size(); ++i) {
            if (g.links[i].empty()) {
                g.links[i].assign(tmp.begin(), tmp.end());
            }
        }
    }
    // Algo
    for (int i = 0; i < max_iter; ++i) {
        change = 0;
        do_page_rank();
        if (change < min_delta) {
            break;
        }
        // printf("Iter %d, change: %Lf\n", i, change);
    }

#ifdef DEBUG
    printPR();
#endif

    return 0;
}