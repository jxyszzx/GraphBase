#include <iostream>
#include <set>
#include <queue>
#include <cmath>
#include <algorithm>
#include "graph.h"
#include "omp.h"

// #define DEBUG
// #define EXP

using namespace std;

#ifdef EXP
clock_t t_start;

void init_time() {
    t_start=clock();
}

void get_time_cost(std::string name) {
    double t_cost = (double)(clock()-t_start)/CLOCKS_PER_SEC;
    printf("%s time cost: %lf\n", name.c_str(), t_cost);
}
#endif

Graph g;
vector<set<int> > graph, graph_reverse;
vector<set<int> > scc;
set<int> vertices_;

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
        if (input_graph[v].empty()) {
            to_delete.push(v);
            vertices_.erase(v);
        }
    }

    while (!to_delete.empty()) {
        int u = to_delete.front();
        to_delete.pop();
        scc.push_back(set<int>{u});

        for (auto v : vertices_) {
            input_graph[v].erase(u);            
            if (input_graph[v].empty()) {
                to_delete.push(v);
                vertices_.erase(v);
            }
        }
        transpose_graph[u].clear();
    }
}

void updateScc(const set<int>& verts) {
    #pragma omp critical
    scc.push_back(verts);
}

void bfs(const set<int>& cur_verts, const vector<set<int> >& g, int pivot, set<int>& ret_verts) {
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
    string filename(argv[1]);
    filename = "../datasets/" + filename;
    g = Graph(false, filename);

#ifdef EXP
    init_time();
#endif

    // Init
    init();
    trim(graph, graph_reverse);
    trim(graph_reverse, graph);

#ifdef DEBUG
    puts("After trim");
    for (auto v : vertices_) {
        printf("%d ", v);
    }
    puts("");
#endif
    
    // Algo
    forward_back_algo(vertices_);

#ifdef DEBUG
    printf("%d\n", scc.size());
    // printSCC();
#endif

#ifdef EXP
    get_time_cost("scc");
#endif

    return 0;
}