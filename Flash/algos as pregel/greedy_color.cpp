#include <map>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "graph.h"
#include "omp.h"

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

int *color_;
omp_lock_t *lock_, global_lock_;
int *in_deg;

void printGC() {
    // puts("in");
    printf("GC := { ");
    for (int i = 0; i < N; ++i) {
        printf("[%d] ", color_[i]);
    }
    puts(" }");
}


void graph_coloring() {
    int *_vertices = new int[N](), _cnt = 0;
    int *_new_vertices = new int[N](), _new_cnt = 0;
    for (int i = 0; i < N; ++i) {
        if (rpos[i+1] == rpos[i]) {
            _vertices[_cnt++] = i;
        }
    }

    do {
// init_time("round");
        _new_cnt = 0;

        #pragma omp parallel for
        for (int i = 0; i < _cnt; ++i) {
            const int &u = _vertices[i];

            const int &_num = rpos[u+1]-rpos[u]+1;

            bool *vis = new bool[_num];
            memset(vis, false, sizeof(bool) * (_num));

            // get_color
            for (long long e_idx = rpos[u]; e_idx < rpos[u+1]; ++e_idx) {
                const int &v = redges[e_idx];
                if (color_[v] >= _num || vis[color_[v]]) continue;
                // if (color_[v] >= _num) continue;
                // if (color_[v] < 0) {
                //     puts("Err");
                    // cout << u << ' ' << v << ' ' << color_[v] << endl;
                // } else {
                    vis[color_[v]] = true;
                // }
            }
            int _min_c = 0;
            for (; _min_c < _num; ++_min_c) {
                if (!vis[_min_c]) {
                    break;
                }
            }
            color_[u] = _min_c;

// printf("Id %d : %d\n", u, _min_c);

            // influence
            for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
                const int &v = edges[e_idx];
                // #pragma omp critical(v)
                // {
                    omp_set_lock(&lock_[v]);
                    --in_deg[v];
                    if (in_deg[v] == 0) {
                        #pragma omp critical
                        // omp_set_lock(&global_lock_);
                        _new_vertices[_new_cnt++] = v;
                        // omp_unset_lock(&global_lock_);
                    }
                    omp_unset_lock(&lock_[v]);
            }

            delete []vis;
        }

        /*#pragma omp parallel for
        for (int i = 0; i < _cnt; ++i) {
            const int &u = _vertices[i];
            for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
                const int &v = edges[e_idx];
                if (in_deg[v] == -1) continue;
                // #pragma omp critical
                if (in_deg[v] == 0) {
                    omp_set_lock(&global_lock_);
                    if (in_deg[v] == 0) {
                        _new_vertices[_new_cnt++] = v;
                        in_deg[v] = -1;
                    }
                    omp_unset_lock(&global_lock_);
                }
            }
        }*/
        memcpy(_vertices, _new_vertices, sizeof(int) * _new_cnt);

        _cnt = _new_cnt;
// get_time_cost("round");
    } while (_cnt != 0);
    
}

void graph_coloring_lp() {
    bool *update_pre_iter = new bool[N];
    for (int i = 0; i < N; ++i) {
        if (rpos[i+1] == rpos[i]) {
            update_pre_iter[i] = true;
        } else {
            update_pre_iter[i] = false;
        }
    }

    bool update = false;

    do {
// init_time("round");
        update = false;

        #pragma omp parallel for reduction(| : update)
        for (int u = 0; u < N; ++u) {
            if (!update_pre_iter[u]) continue;
            update_pre_iter[u] = false;

            const int &_num = rpos[u+1]-rpos[u]+1;

            bool *vis = new bool[_num];
            memset(vis, false, sizeof(bool) * (_num));

            // get_color
            for (long long e_idx = rpos[u]; e_idx < rpos[u+1]; ++e_idx) {
                const int &v = redges[e_idx];
                if (color_[v] >= _num || vis[color_[v]]) continue;
                // if (color_[v] >= _num) continue;
                // if (color_[v] < 0) {
                //     puts("Err");
                    // cout << u << ' ' << v << ' ' << color_[v] << endl;
                // } else {
                    vis[color_[v]] = true;
                // }
            }
            int _min_c = 0;
            for (; _min_c < _num; ++_min_c) {
                if (!vis[_min_c]) {
                    break;
                }
            }
            color_[u] = _min_c;

// printf("Id %d : %d\n", u, _min_c);

            // influence
            for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
                const int &v = edges[e_idx];
                // #pragma omp critical(v)
                // {
                    omp_set_lock(&lock_[v]);
                    --in_deg[v];
                    omp_unset_lock(&lock_[v]);
                    if (in_deg[v] == 0) {
                        // omp_set_lock(&global_lock_);
                        update_pre_iter[v] = true;
                        update |= true;
                        // omp_unset_lock(&global_lock_);
                    }
            }

            delete []vis;
        }

        /*#pragma omp parallel for
        for (int i = 0; i < _cnt; ++i) {
            const int &u = _vertices[i];
            for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
                const int &v = edges[e_idx];
                if (in_deg[v] == -1) continue;
                // #pragma omp critical
                if (in_deg[v] == 0) {
                    omp_set_lock(&global_lock_);
                    if (in_deg[v] == 0) {
                        _new_vertices[_new_cnt++] = v;
                        in_deg[v] = -1;
                    }
                    omp_unset_lock(&global_lock_);
                }
            }
        }*/
// get_time_cost("round");
    } while (update);
    
}

void create_graph() {
    vector<vector<int> > prev(N, vector<int>()), succ(N, vector<int>());
    for (int u = 0; u < N; ++u) {
        for (auto v : g_.links[u]) {
            if (v <= u) continue;
            if (g_.links[u].size() <= g_.links[v].size()) {
                succ[u].push_back(v);
                prev[v].push_back(u);
                
            } else {
                succ[v].push_back(u);
                prev[u].push_back(v);
            }
        }
    }

// puts("prev");
// for (int i = 0; i < N; ++i) {
//     printf("[%d] : ", i);
//     for (auto v : prev[i]) {
//         printf("%d ", v);
//     }
//     puts("");
// }
// puts("succ");
// for (int i = 0; i < N; ++i) {
//     printf("[%d] : ", i);
//     for (auto v : succ[i]) {
//         printf("%d ", v);
//     }
//     puts("");
// }

    // succ
	pos = new long long[N + 1];
    long long _cnt = 0;
    for (int i = 0; i < N; ++i) {
        pos[i] = _cnt;
        _cnt += succ[i].size();
    }
    pos[N] = _cnt;

    edges = new int[_cnt];
    _cnt = 0;
    for (auto nbr : succ) {
        for (auto v : nbr) {
            edges[_cnt++] = v;
        }
    }

    // prev
in_deg = new int[N+1];
    rpos = new long long[N + 1];
    _cnt = 0;
    for (int i = 0; i < N; ++i) {
        rpos[i] = _cnt;
        _cnt += prev[i].size();
in_deg[i] = prev[i].size();
    }
    rpos[N] = _cnt;

    redges = new int[_cnt];
    _cnt = 0;
    for (auto nbr : prev) {
        for (auto v : nbr) {
            redges[_cnt++] = v;
        }
    }

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

    int NUM = atoi(argv[1]);
    omp_set_num_threads(NUM);

    get_time_cost("read");

    // Init
	N = g_.links.size();
    create_graph();
    color_ = new int[N+1];
    memset(color_, -1, sizeof(int) * (N+1));

    lock_ = new omp_lock_t[N];
	#pragma omp parallel for
	for (int i = 0; i < N; ++i) {
		omp_init_lock(&lock_[i]);
	}
    omp_init_lock(&global_lock_);


    // Algo
init_time("algo");
    graph_coloring_lp();
get_time_cost("algo");

for (int i = 0; i < N; ++i) {
    if (color_[i] == -1) {
        puts("Err -1");
    }
    for (int j = 0; j < g_.links[i].size(); ++j) {
        int v = g_.links[i][j];
        if (i != v && color_[i] == color_[v]) {
            puts("Err same");
        }
    }
}
// printf("%d\n", ans);

#ifdef DEBUG
    printGC();
#endif

    delete []pos;
    delete []rpos;
    delete []edges;
    delete []redges;
    delete []color_;
    #pragma omp parallel for
	for (int i = 0; i < N; ++i) {
		omp_destroy_lock(&lock_[i]);
	}
    omp_destroy_lock(&global_lock_);
    delete []lock_;

    return 0;
}