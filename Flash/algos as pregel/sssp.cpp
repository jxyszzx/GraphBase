#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <omp.h>
#include "graph.h"
using namespace std;

#define INF 0x7f7f7f7f
// #define DEBUG

std::map<std::string, double> t_starts_;
inline void init_time(std::string name) {
    t_starts_[name] = omp_get_wtime();
}
inline void get_time_cost(std::string name) {
    double t_cost = omp_get_wtime()-t_starts_[name];
    printf("%s time cost: %lf\n", name.c_str(), t_cost);
}

bool updated;
Graph g;

int *edges, *g_label, *dist, N;
long long *pos;
bool *vis;

void create_graph(int source_node) {
    N = g.links.size();

    pos = new long long[N + 1];
    dist = new int[N];
    g_label = new int[N];
    vis = new bool[N];

    for (int i = 0; i < N; ++i) {
        if (i == source_node) {
            dist[i] = g_label[i] = 0;
            vis[i] = true;
            continue;
        }
        dist[i] = g_label[i] = INF;
        vis[i] = false;
    }

    long long cnt = 0;
    for (int i = 0; i < N; ++i) {
        pos[i] = cnt;
        cnt += g.links[i].size();
    }
    pos[g.links.size()] = cnt;

    edges = new int[cnt];
    cnt = 0;
    for (auto nbr : g.links) {
        for (auto v : nbr) {
            edges[cnt++] = v;
        }
    }
}

void printSSSP() {
    printf("SSSP := { ");
    for (int i = 0; i < N; ++i) {
        if (g_label[i] == INF) {
            g_label[i] = -1;
        }
        printf("[%d]%s", g_label[i], i+1==N ? " }\n" : ",");
    }
}

void do_sssp() {
    bool updated;

    omp_set_num_threads(32);
    do {
        init_time("update");

        updated = false;
        #pragma omp parallel
        {
            #pragma omp for firstprivate(pos, edges, vis, g_label) schedule(dynamic, 128) reduction(| : updated)
            for (int i = 0; i < N; ++i) {
                if (vis[i]) {
                    for (long long e_idx = pos[i]; e_idx < pos[i+1]; ++e_idx) {
                        int u = edges[e_idx];
                        if (dist[u] == INF) {
                            dist[u] = g_label[i] + 1;
                            updated |= true;
                        }
                    }
                }
                vis[i] = false;
            }

            #pragma omp single nowait
            {
                get_time_cost("update");
                init_time("main");
            }

            #pragma omp for
            for (int i = 0; i < N; ++i) {
                if (g_label[i] != dist[i]) {
                    g_label[i] = dist[i];
                    vis[i] = true;
                } else {
                    vis[i] = false;
                }
            }
        }
        get_time_cost("main");

    } while (updated);
    // printSSSP();
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

#ifdef DEBUG   
    g.display();
#endif
    // puts("ok");

    // Init
    int source_node = 0;
    create_graph(source_node);
    // Algo
    init_time("algo");
    
    do_sssp();

    get_time_cost("algo");

#ifdef DEBUG
    printSSSP();
#endif

    return 0;
}