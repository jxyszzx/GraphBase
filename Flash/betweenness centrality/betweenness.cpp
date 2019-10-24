#include <iostream>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "graph.h"
#include "omp.h"

#define DEBUG_INFO
#define TIME_INFO
// #define THREAD_INFO

#ifdef TIME_INFO
std::map<std::string, clock_t> t_starts_;
void init_time(std::string name) {
    t_starts_[name] = clock();
}
void get_time_cost(std::string name) {
    double t_cost = (double)(clock()-t_starts_[name])/CLOCKS_PER_SEC;
    printf("%s time cost: %lf\n", name.c_str(), t_cost);
}
#endif

using namespace std;


// struct Node {
//     int node;
//     int dist;
//     Node(int node_, int dist_) : node(node_), dist(dist_) {}
// };

// bool operator < (cosnt Node& a, const Node& b) {
//     return a.dist < b.dist;
// }

Graph g_;

double *centrality;

int *edges, N;
long long *pos;

void create_graph() {
    N = g_.links.size();

    pos = new long long[N + 1];

    long long cnt = 0;
    for (int i = 0; i < N; ++i) {
        pos[i] = cnt;
        cnt += g_.links[i].size();
    }
    pos[N] = cnt;

    edges = new int[cnt];
    cnt = 0;
    for (auto nbr : g_.links) {
        for (auto v : nbr) {
            edges[cnt++] = v;
        }
    }
}

void printBC() {
    for (int i = 0; i < N; ++i) {
        printf("Node %d: %lf\n", i, centrality[i]);
    }
}

void do_centrality() {
    omp_set_num_threads(32);
    #pragma omp parallel for schedule(dynamic, 128)
    for (int s = 0; s < N; ++s) {
        queue<int> que;
        // vector<int> s;
        stack<int> S;
        
        vector<vector<int> > parents(N, vector<int>());
        int *num_paths = new int[N], *dist = new int[N];
        memset(num_paths, 0, sizeof(int)*N);
        memset(dist, -1, sizeof(int)*N);

        num_paths[s] = 1;
        dist[s] = 0;
        que.push(s);

        while (!que.empty()) {
            int u = que.front();
            que.pop();
            // s.push_back(u);
            S.push(u);
            
            for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
                int v = edges[e_idx];
                if (dist[v] == -1) {
                    que.push(v);
                    dist[v] = dist[u] + 1;
                }
                if (dist[v] == dist[u] + 1) {
                    num_paths[v] += num_paths[u];
                    parents[v].push_back(u);
                }
            }
        }

        double *delta = new double[N];
        for (int i = 0;  i < N; ++i) {
            delta[i] = 0;
        }
        while (!S.empty()) {
            int u = S.top();
            S.pop();
            double coeff = (1+delta[u])/num_paths[u];
            for (auto p : parents[u]) {
                delta[p] += num_paths[p]*coeff;
            }
            if (u != s) {
                #pragma omp critical(u)
                centrality[u] += delta[u];
            }
        }

        delete[] num_paths;
        delete[] dist;
        delete[] delta;
    }
}


int main(int argc, char *argv[])
{
init_time("read");

    if (argc == 2) {
        g_ = Graph(true, atoi(argv[1]), "");
    } else if (argc == 3) {
        // assert(atoi(argv[1]) == 0);
        string filename(argv[2]);
        filename = "../datasets/" + filename;
        g_ = Graph(true, 0, filename);
    }

get_time_cost("read");

    create_graph();
    // init centrality
    centrality = new double[N];
    for (int i = 0; i < N; ++i) {
        centrality[i] = 0.0;
    }

#ifdef THREAD_INFO
    if( omp_get_max_threads() > 1 ) {
        printf("Running in parallel...   YES\n");
        printf("Threads available: %d\n", omp_get_max_threads() );
    } else {
        printf("Running in parallel...   NO\n");
    }
#endif

#ifdef DEBUG_INFO
    printf("Beginning betweenness centrality computation...\n");
#endif

init_time("centrality_time");

    do_centrality();

get_time_cost("centrality_time");

#ifdef DEBUG_INFO
    printf("Computing complete\n");
#endif

#ifdef DEBUG_INFO
    printBC();
#endif


    delete[] centrality;
    delete[] pos;
    delete[] edges;

    return 0;
}