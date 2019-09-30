#include <iostream>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <cmath>
#include <algorithm>
#include "graph.h"
#include "omp.h"

// #define DEBUG_INFO
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
vector<double> centrality;

void updateBC(int u, double add) {
    #pragma omp critical
    centrality[u] += add;
}

void printBC() {
    for (int i = 0; i < centrality.size(); ++i) {
        printf("Node %d: %lf\n", i, centrality[i]);
    }
}

void do_centrality() {
    const int &n = g_.links.size();

    #pragma omp parallel for
    for (int s = 0; s < n; ++s) {
        queue<int> que;
        // vector<int> s;
        stack<int> S;
        vector<int> num_paths(n, 0), dist(n, -1);
        vector<vector<int> > parents(n, vector<int>());
        num_paths[s] = 1;
        dist[s] = 0;
        que.push(s);

        while (!que.empty()) {
            int u = que.front();
            que.pop();
            // s.push_back(u);
            S.push(u);
            
            for (auto v : g_.links[u]) {
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

        vector<double> delta(n, 0);
        while (!S.empty()) {
            int u = S.top();
            S.pop();
            double coeff = (1+delta[u])/num_paths[u];
            for (auto p : parents[u]) {
                delta[p] += num_paths[p]*coeff;
            }
            if (u != s) {
                updateBC(u, delta[u]/2);
            }
        }
    }
}


int main(int argc, char *argv[])
{
    // usage and input
    if(argc != 2) {  
        printf("usage: ./betweenness [ingraph]\n");
        return -1;
    }

#ifdef TIME_INFO
    init_time("total_time");
#endif

    // read graph
    string filename(argv[1]);
    filename = "../datasets/" + filename;
    g_ = Graph(false, filename);

    // init centrality
    centrality.assign(g_.links.size(), 0);

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
#ifdef TIME_INFO
    init_time("centrality_time");
#endif
    do_centrality();
#ifdef TIME_INFO
    get_time_cost("centrality_time");
#endif
#ifdef DEBUG_INFO
    printf("Computing complete\n");
#endif

#ifdef DEBUG_INFO
    printBC();
#endif

#ifdef TIME_INFO
    get_time_cost("total_time");
#endif

    return 0;
}