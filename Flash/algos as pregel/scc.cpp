#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <cmath>
#include <algorithm>
#include "graph.h"
#include "omp.h"

// #define DEBUG

std::map<std::string, clock_t> t_starts_;
void init_time(std::string name) {
    t_starts_[name] = clock();
}
void get_time_cost(std::string name) {
    double t_cost = (double)(clock()-t_starts_[name])/CLOCKS_PER_SEC;
    printf("%s time cost: %lf\n", name.c_str(), t_cost);
}

using namespace std;

Graph g;
vector<set<int> > graph, graph_reverse;
vector<set<int> > scc;
set<int> vertices_;

int N;
long long *pos;
int* edges;

void create_graph() {
    N = g.links.size();

    pos = new long long[N + 1];

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

inline void set_Inter(const set<int>& s1, const set<int>& s2, set<int>& s3) {
    set_intersection(s1.begin(),s1.end(),s2.begin(),s2.end(),inserter(s3,s3.begin()));
}

inline void set_Union(const set<int>& s1, const set<int>& s2, set<int>& s3) {
    set_union(s1.begin(),s1.end(),s2.begin(),s2.end(),inserter(s3,s3.begin()));
}

inline void set_Diff(const set<int>& s1, const set<int>& s2, set<int>& s3) {
    set_difference(s1.begin(),s1.end(),s2.begin(),s2.end(),inserter(s3,s3.begin()));
}

void init() {
    graph.resize(g.label.size(), set<int>());
    graph_reverse.resize(g.label.size(), set<int>());
    for (int i = 0; i < g.links.size(); ++i) {
        for (auto j : g.links[i]) {
            graph[i].insert(j);
            graph_reverse[j].insert(i);
        }
    }

    for (int i = 0; i < g.label.size(); ++i) {
        vertices_.insert(i);
    }
}





void trim(vector<set<int> >& input_graph, vector<set<int> >& transpose_graph) {
    queue<int> to_delete;

    for (auto v : vertices_) {
// printf("%d -\n", v);
// if (v >= input_graph.size()) {
//     puts("what the hell - 1");
// }
// cout << v << endl;
        if (input_graph[v].empty()) {
            // puts("cool");
            to_delete.push(v);
        }
    }

// puts("1");
    while (!to_delete.empty()) {
        int u = to_delete.front();
        to_delete.pop();

        vertices_.erase(u);
        scc.push_back(set<int>{u});
// puts("2");
        for (auto v : vertices_) {
// if (v >= input_graph.size()) {
//     puts("what the hell - 2");
// }
            if (!input_graph[v].empty()) {
                input_graph[v].erase(u);            
                if (input_graph[v].empty()) {
                    to_delete.push(v);
                }
            }
        }
// puts("3");
// if (u >= transpose_graph.size()) {
//     puts("what the hell - 3");
// }
        transpose_graph[u].clear();
    }
}

void updateScc(const set<int>& verts) {
    #pragma omp critical
    scc.push_back(verts);
}

void bfs(const set<int>& cur_verts, const vector<set<int> >& g, int pivot, set<int>& ret_verts) {
// puts("bfs in");
    queue<int> que;

    que.push(pivot);
    ret_verts.insert(pivot);
    while (!que.empty() && ret_verts.size() != cur_verts.size()) {
        int u = que.front();
        que.pop();

        set<int> inter, diff;
        set_Inter(g[u], cur_verts, inter);
        set_Diff(inter, ret_verts, diff);
        for (auto v : diff) {
            que.push(v);
            ret_verts.insert(v);
        }
    }
// puts("bfs out");
}

void forward_back_algo(set<int> verts) {
    if (verts.size() == 0) {
        return;
    }
    if (verts.size() == 1) {
        updateScc(verts);
        return;
    }
    
    set<int>::iterator it = verts.begin();
    int pivot = *it;
    

    set<int> s1, s2, s_inter, s_union, s3, s1_left, s2_left;
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            bfs(verts, graph, pivot, s1);
        }
        #pragma omp section
        {
            bfs(verts, graph_reverse, pivot, s2);
        }
    }

    set_Inter(s1, s2, s_inter);
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            set_Union(s1, s2, s_union);
            set_Diff(verts, s_union, s3);
        }
        #pragma omp section
        {
            set_Diff(s1, s_inter, s1_left);
        }
        #pragma omp section
        {
            set_Diff(s2, s_inter, s2_left);
        }
    }
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            forward_back_algo(s3);
        }
        #pragma omp section
        {
            forward_back_algo(s1_left);
        }
        #pragma omp section
        {
            forward_back_algo(s2_left);
        }
        #pragma omp section
        {
            updateScc(s_inter);
        }
    }
}

void printSCC() {
    for (int i = 0; i < scc.size(); ++i) {
        printf("CC %d: ", i);
        for (auto x : scc[i]) {
            printf("%d ", x);
        }
        puts("");
    }
}

int main(int argc, char *argv[])
{
    init_time("read");

    if (argc == 2) {
        g = Graph(false, atoi(argv[1]), "");
    } else if (argc == 3) {
        // assert(atoi(argv[1]) == 0);
        string filename(argv[2]);
        filename = "../datasets/" + filename;
        g = Graph(false, 0, filename);
    }
    get_time_cost("read");

#ifdef DEBUG   
    g.display();
#endif

    // Init
// puts("init start");
    init();
// puts("init end");

// for (auto v : vertices_) {
//     cout << v << endl;
// }
// puts("end vertices");
    trim(graph, graph_reverse);

// puts("trim1 end");
    trim(graph_reverse, graph);
// puts("trim2 end");
    
    create_graph();

    init_time("algo");
    // Algo
    omp_set_num_threads(32);
    forward_back_algo(vertices_);

    get_time_cost("algo");

#ifdef DEBUG
    printSCC();
#endif

    return 0;
}