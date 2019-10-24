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

omp_lock_t *lock_;

int *G_scc;

void printSCC() {
    printf("SCC := { ");
    for (int i = 0; i < N; ++i) {
        printf("[%d] ", G_scc[i]);
    }
    puts(" }");
}

int color_cc() {
    int ret = 0;
    int v_num = N;
    int *vertices = new int[N+1];
    G_scc = new int[N+1];

    // Init
    #pragma omp for
    for (int i = 0; i < N; ++i) {
        vertices[i] = i;
    }
    memset(G_scc, -1, sizeof(int) * (N+1));
    
    // local
    int *cur_color = new int[N+1];
    bool *update_pre_iter = new bool[N+1];
    int *Que = new int[N+1], p = 0;

init_time("real algo");

    while (v_num > 0) {
        #pragma omp parallel for 
        for (int u_idx = 0; u_idx < v_num; ++u_idx) {
            const int &u = vertices[u_idx];
            cur_color[u] = u;
            update_pre_iter[u] = true;
        }
        
        int local_update;
        do {
            local_update = false;

            #pragma omp parallel for reduction(| : local_update)
            for (int u_idx = 0; u_idx < v_num; ++u_idx) {
                const int &u = vertices[u_idx];
                if (!update_pre_iter[u]) continue;
                update_pre_iter[u] = false;
                for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
                    const int &v = edges[e_idx];
                    if (G_scc[v] == -1 && cur_color[u] < cur_color[v]) {
                        omp_set_lock(&lock_[v]);
                        if (cur_color[u] < cur_color[v]) {
                            cur_color[v] = cur_color[u];
                        }
                        omp_unset_lock(&lock_[v]);
                        update_pre_iter[v] = true;
                        local_update |= true;

                        // printf("(%d, %d) : %d, %d\n", u, v, cur_color[u], cur_color[v]);
                    }
                }
            }

// for (int i = 0; i < N; ++i) {
//     cout << i << " : " << cur_color[i] << endl;
// }
// printSCC();
        } while (local_update);

        // for (int u = 0; u < N; ++u) {
        //     for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
        //         const int &v = edges[e_idx];
        //         if (G_scc[v] == -1 && cur_color[u] < cur_color[v]) {
                    
        //             printf("(%d, %d) : %d, %d\n", u, v, cur_color[u], cur_color[v]);
        //         }
        //     }
        // }

/*
        // ---BFS VERSOIN
        p = 0;
        for (int u_idx = 0; u_idx < v_num; ++u_idx) {
            const int &u = vertices[u_idx];
            if (cur_color[u] == u) {
                G_scc[u] = u;
                Que[p++] = u;
            }
        }
// printf("Que %d", p);
// for (int i = 0; i < p; ++i) {
//     printf("%d ", Que[i]);
// }      
// puts("");
        #pragma omp parallel for reduction (+ : ret)
        for (int u_idx = 0; u_idx < p; ++u_idx) {
            const int &u = Que[u_idx];

            int *Que_bfs = new int[N+1], p_bfs = 0;
            Que_bfs[p_bfs++] = u;

            for (int cur_bfs = 0; cur_bfs < p_bfs; ++cur_bfs) {
                const int &x = Que_bfs[cur_bfs];
                
                for (long long e_idx = rpos[x]; e_idx < rpos[x+1]; ++e_idx) {
                    const int &y = redges[e_idx];
                    if (G_scc[y] == -1 && cur_color[y] == cur_color[u]) {
                        G_scc[y] = cur_color[u];
                        Que_bfs[p_bfs++] = y;
                    }
                }
            }
            ret += 1;

            delete []Que_bfs;
        }
// puts("new");
// for (int i = 0; i < N; ++i) {
//     cout << i << " : " << cur_color[i] << endl;
// }
// printSCC();

*/
        // ---LP VERSOIN
        #pragma omp parallel for reduction(+ : ret)
        for (int u_idx = 0; u_idx < v_num; ++u_idx) {
            const int &u = vertices[u_idx];
            if (cur_color[u] == u) {
                G_scc[u] = u;
                update_pre_iter[u] = true;
                ret += 1;
            }
        }

        do {
            local_update = false;

            #pragma omp parallel for reduction(| : local_update)
            for (int u_idx = 0; u_idx < v_num; ++u_idx) {
                const int &u = vertices[u_idx];
                if (!update_pre_iter[u]) continue;
                update_pre_iter[u] = false;

                for (long long e_idx = rpos[u]; e_idx < rpos[u+1]; ++e_idx) {
                    const int &v = redges[e_idx];
                    if (G_scc[v] == -1 && cur_color[v] == G_scc[u]) {
                        G_scc[v] = G_scc[u];
                        update_pre_iter[v] = true;
                        local_update |= true;
                        // printf("(%d, %d) : %d, %d\n", u, v, cur_color[u], cur_color[v]);
                    }
                }
            }
        } while (local_update);

        int p1 = 0;
        for (int u_idx = 0; u_idx < v_num; ++u_idx) {
            const int &u = vertices[u_idx];
            if (G_scc[u] == -1) {
                vertices[p1++] = u;
                // vertices[p1++] = vertices[u_idx];
            }
        }
        v_num = p1;
printf("%d\n", v_num);
    }

    get_time_cost("real algo");
    
    return ret;
}

void create_graph() {	
    int max_deg = 0;

	pos = new long long[N + 1];
    long long _cnt = 0;
    for (int i = 0; i < N; ++i) {
        pos[i] = _cnt;
        _cnt += g_.links[i].size();

        if (max_deg < g_.links[i].size()) {
            max_deg = g_.links[i].size();
        }
       
    }
    pos[N] = _cnt;

    edges = new int[_cnt];
    _cnt = 0;
    for (auto nbr : g_.links) {
        for (auto v : nbr) {
            edges[_cnt++] = v;
        }
    }


    // Put Info
    printf("#Node: %d, #Edge: %lld, Max deg: %d, avg.deg: %lf\n", N, _cnt, max_deg, 2.0*_cnt/N);
}

void create_graph_reverse() {
    vector<vector<int> > graph_reverse(N, vector<int>());
    for (int u = 0; u < g_.links.size(); ++u) {
        for (auto v : g_.links[u]) {
            graph_reverse[v].push_back(u);
        }
    }

	rpos = new long long[N + 1];
    long long _cnt = 0;
    for (int i = 0; i < N; ++i) {
        rpos[i] = _cnt;
        _cnt += graph_reverse[i].size();
    }
    rpos[N] = _cnt;

    redges = new int[_cnt];
    _cnt = 0;
    for (auto nbr : graph_reverse) {
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
        g_ = Graph(false, atoi(argv[2]), "");
    } else if (argc == 4) {
        // assert(atoi(argv[1]) == 0);
        string filename(argv[3]);
        filename = "../datasets/" + filename;
        g_ = Graph(false, 0, filename);
    }

    int NUM = atoi(argv[1]);
    omp_set_num_threads(NUM);

    get_time_cost("read");

    // Init
	N = g_.links.size();
    create_graph();
    create_graph_reverse();
	
	lock_ = new omp_lock_t[N];
	#pragma omp parallel for
	for (int i = 0; i < N; ++i) {
		omp_init_lock(&lock_[i]);
	}

    // Algo
init_time("algo");
    int ans = color_cc();
get_time_cost("algo");

printf("%d\n", ans);

#ifdef DEBUG
    printSCC();
#endif
    return 0;
}