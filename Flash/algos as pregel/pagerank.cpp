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

double* pagerank, *new_pagerank, *page_matter;

Graph g;

double damping_factor, min_delta, damping_value;

int N;
long long *pos;
int* edges;

void create_graph() {
    N = g.links.size();

    pos = new long long[N + 1];
    pagerank = new double[N];
    new_pagerank = new double[N];
    page_matter = new double[N];

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

    while (max_iter--) {
// init_time("one round");

        double change = 0;
        
        // init_time("pagerank");

        #pragma omp parallel
        {
            #pragma omp for schedule(dynamic,128) reduction(+ : change)
            for (int i = 0; i < N; ++i) {
                double _PR = 0.0;
                for (long long e_idx = pos[i]; e_idx < pos[i+1]; ++e_idx) {
                    int u = edges[e_idx];
                    _PR += page_matter[u];
                    // pagerank[u] / (pos[u+1]-pos[u]);
                }
                new_pagerank[i] = damping_value + _PR * damping_factor;
                change += fabs(new_pagerank[i] - pagerank[i]);
            }
        // #pragma omp single
        // {
        // get_time_cost("pagerank");
        // init_time("update");

        // }

            #pragma omp for
            for (int i = 0; i < N; ++i) {
// printf("%d: %LF, %Lf\n", i, pagerank[i], new_pagerank[i]);
                pagerank[i] = new_pagerank[i];
                page_matter[i] = pagerank[i]/(pos[i+1]-pos[i]);
            }
            
        }
        // get_time_cost("update");

        if (change <= min_delta) {
            break;
        }
// get_time_cost("one round");

// printf("Iter %d, change: %lf\n", max_iter, change);
    }
}

void printPR() {
    printf("pagerank := { ");
    for (int i = 0; i < N; ++i) {
        printf("[%lf] ", pagerank[i]);
    }
    puts(" }");
}

int main(int argc, char *argv[])
{
    /*  [./wcc 32 1]
        [./wcc 32 0 sample.txt]
    */
    init_time("read");

    if (argc == 3) {
        g = Graph(true, atoi(argv[2]), "");
    } else if (argc == 4) {
        // assert(atoi(argv[1]) == 0);
        string filename(argv[3]);
        filename = "../datasets/" + filename;
        g = Graph(true, 0, filename);
    }

    int NUM = atoi(argv[1]);
    omp_set_num_threads(NUM);

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
        page_matter[i] = pagerank[i]/(pos[i+1]-pos[i]);
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