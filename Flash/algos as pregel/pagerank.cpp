#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <set>
#include <omp.h>
#include "graph.h"
using namespace std;

// #define DEBUG

std::map<std::string, double> t_starts_;
inline void init_time(std::string name) {
    t_starts_[name] = omp_get_wtime();
}
inline void get_time_cost(std::string name) {
    double t_cost = omp_get_wtime()-t_starts_[name];
    printf("%s time cost: %lf\n", name.c_str(), t_cost);
}

long double* pagerank, *new_pagerank;

Graph g;

double damping_factor, min_delta, damping_value;

int N;
long long *pos;
int* edges;

void create_graph() {
    N = g.links.size();

    pos = new long long[N + 1];
    pagerank = new long double[N];
    new_pagerank = new long double[N];

    long long cnt = 0;
    for (int i = 0; i < N; ++i) {
        pos[i] = cnt;
        cnt += g.links[i].size();
    }
    pos[N] = cnt;

    edges = new int[cnt];
    cnt = 0;
    for (auto nbr : g.links) {
        for (auto v : nbr) {
            edges[cnt++] = v;
        }
    }
}

void do_page_rank(int max_iter) {
    omp_set_num_threads(32);
    while (max_iter--) {
// init_time("one round");
        double change = 0;
        #pragma omp parallel
        {
            #pragma omp for reduction(+ : change)
            for (int i = 0; i < N; ++i) {
                double _PR = 0.0;
                for (long long e_idx = pos[i]; e_idx < pos[i+1]; ++e_idx) {
                    int u = edges[e_idx];
                    _PR += pagerank[u] / (pos[u+1]-pos[u]);
                }
                new_pagerank[i] = damping_value + _PR * damping_factor;
                change += fabs(new_pagerank[i] - pagerank[i]);
            }
            #pragma omp for
            for (int i = 0; i < N; ++i) {
// printf("%d: %LF, %Lf\n", i, pagerank[i], new_pagerank[i]);
                pagerank[i] = new_pagerank[i];
            }    
        }
        if (change < min_delta) {
            break;
        }
// get_time_cost("one round");
// printf("Iter %d, change: %lf\n", max_iter, change);
    }
}

void printPR() {
    printf("pagerank := { ");
    for (int i = 0; i < N; ++i) {
        printf("[%Lf] ", pagerank[i]);
    }
    puts(" }");
}

int main(int argc, char *argv[])
{
    init_time("read");

    if (argc == 2) {
        g = Graph(true, atoi(argv[1]), "");
    } else if (argc == 3) {
        // assert(atoi(argv[1]) == 0);
        string filename(argv[2]);
        filename = "../datasets/" + filename;
        g = Graph(true, 0, filename);
    }

    get_time_cost("read");

    // Init
    int max_iter = 100;
    min_delta = 1e-3;
    damping_factor = 0.85;
    damping_value = (1-damping_factor)/g.label.size();
  
    // deal with those nodes with no out-degree
    for (int i = 0; i < g.links.size(); ++i) {
        if (g.links[i].empty()) {
            for (int j = 0; j < g.links.size(); ++j) {
                g.links[i].push_back(j);
                g.links[j].push_back(i);
            }
        }
    }

    create_graph();

    #pragma omp parallel for
    for (int i = 0; i < g.label.size(); ++i) {
        pagerank[i] = 1.0 / N;
    }

init_time("algo");
    // Algo
    do_page_rank(max_iter);
get_time_cost("algo");

#ifdef DEBUG
    printPR();
#endif

    return 0;
}