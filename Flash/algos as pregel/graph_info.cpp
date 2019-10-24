#include <set>
#include <map>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "graph.h"

// #define DEBUG

std::map<std::string, double> t_starts_;
inline void init_time(std::string name) {
    t_starts_[name] = omp_get_wtime();
}
inline void get_time_cost(std::string name) {
    double t_cost = omp_get_wtime()-t_starts_[name];
    printf("%s time cost: %lf\n", name.c_str(), t_cost);
}

using namespace std;


Graph g_;

int N;
long long *pos, *rpos;
int* edges, *redges;

omp_lock_t *lock_;

int *G_scc;

void graph_info() {
    vector<set<int> > clean_edges(N, set<int>());
    for (int i = 0; i < N; ++i) {
        for (auto v : g_.links[i]) {
            clean_edges[i].insert(v);
        }
        clean_edges[i].erase(i);
    }

    int node_num = 0, max_deg = 0;
    long long total_deg = 0;

    for (int i = 0; i < N; ++i) {
        if (clean_edges[i].empty()) continue;
        ++node_num;
        total_deg += clean_edges[i].size();
        if (max_deg < clean_edges[i].size()) {
            max_deg = clean_edges[i].size();
        }
    }

    // Put Info
    printf("#Node: %d, #Edge: %lld, Max deg: %d, avg.deg: %lf\n", node_num, total_deg/2, max_deg, 1.0*total_deg/node_num);
}


int main(int argc, char *argv[])
{
    /*  [./wcc 32 1]
        [./wcc 32 0 sample.txt]
    */
    init_time("read");

    if (argc == 3) {
        g_ = Graph(true, atoi(argv[2]), "");
    } else if (argc == 4) {
        // assert(atoi(argv[1]) == 0);
        string filename(argv[3]);
        filename = "../datasets/" + filename;
        g_ = Graph(true, 0, filename);
    }

    get_time_cost("read");

    // Init
	N = g_.links.size();
    graph_info();

    return 0;
}