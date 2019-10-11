#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <set>
#include <map>
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

Graph g;
int *edges, *g_label, *cc_label, N;
long long *pos;
bool *vis;

void create_graph() {
    N = g.links.size();

    pos = new long long[N + 1];
    cc_label = new int[N];
    g_label = new int[N];
    vis = new bool[N];

    long long cnt = 0;
    for (int i = 0; i < g.links.size(); ++i) {
        pos[i] = cnt;
        cnt += g.links[i].size();
        g_label[i] = cc_label[i] = i;
        vis[i] = false;
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

/*void do_wcc() {
    bool updated;
    
    omp_set_num_threads(32);
    do {
        init_time("update");

        updated = false;

        #pragma omp parallel firstprivate(N)
        {
            #pragma omp for firstprivate(pos, edges, vis, g_label) 
            for (int i = 0; i < N; ++i) {
                if (vis[i]) continue;
                
                for (int e_idx = pos[i]; e_idx < pos[i+1]; ++e_idx) {
                    int u = edges[e_idx];
                    #pragma omp critical
                    if (cc_label[u] > g_label[i]) {
                        cc_label[u] = g_label[i];
                    }
                }
            }
            #pragma omp single nowait
            {
                get_time_cost("update");
                init_time("main");
            }
            #pragma omp for
            for (int i = 0; i < N; ++i) {
                if (cc_label[i] == g_label[i]) {
                    vis[i] = true;
                } else {
                    g_label[i] = cc_label[i];
                    vis[i] = false;
                    updated = true;
                }
            }
        }

        get_time_cost("main");
    } while (updated);
}*/

void printCC() {
    set<int> comms;
    for (int i = 0; i < N; ++i) {
        comms.insert(g_label[i]);
    }

    printf("社团个数 : %lu = { ", comms.size());
    for (auto c : comms) {
        printf("%d ", c);
    }
    puts("}");

    for (int i = 0; i < N; ++i) {
        printf("[%d]%s", g_label[i], i+1==N ? " }\n" : ",");
    }
}

void do_wcc() {
    bool updated;

    omp_set_num_threads(32);
    do {
        // init_time("update");

        updated = false;
        #pragma omp parallel
        {
            #pragma omp for firstprivate(pos, edges, vis, g_label) schedule(dynamic, 128) reduction(| : updated)
            for (int i = 0; i < N; ++i) {
                int min_num = g_label[i];
                // bool update_local = false;
                for (long long e_idx = pos[i]; e_idx < pos[i+1]; ++e_idx) {
                    int u = edges[e_idx];
                    if (min_num > g_label[u]) {
                        min_num = g_label[u];
                    }
                    // if (cc_label[i] > g_label[u]) {
                    //     cc_label[i] = g_label[u];
                    // }
                }
                if (min_num < g_label[i]) {
                    cc_label[i] = min_num;
                    updated |= true;
                }
            }

            // #pragma omp single nowait
            // {
            //     get_time_cost("update");
            //     init_time("main");
            // }

            #pragma omp for
            for (int i = 0; i < N; ++i) {
                if (g_label[i] != cc_label[i]) {
                    g_label[i] = cc_label[i];
                }
            }
        }

        // get_time_cost("main");
        // printCC();

    } while (updated);
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
    create_graph();

    // cc_label.assign(g.label.begin(), g.label.end());
    // vis.assign(g.label.size(), 0);


#ifdef DEBUG   
    g.display();
#endif
    // puts("ok");

    init_time("algo");
    // Algo
    do_wcc();

    get_time_cost("algo");

#ifdef DEBUG
    printCC();
#endif

    return 0;
}