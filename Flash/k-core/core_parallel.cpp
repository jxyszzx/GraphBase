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
int max_deg, N, *ub, *cur_ub;

int *num_;
bool *ub_update;
omp_lock_t *lock_;

// Only for get_estimate() 
int *cnt_;

int *edges;
long long *pos;

int get_estimate(const int &u, const int &origin_ub) {
	memset(cnt_, 0, sizeof(int)*(origin_ub+1));

	for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
		const int &v = edges[e_idx];
		++cnt_[min(origin_ub, ub[v])];
	}

	num_[u] = 0;
	for (int deg = origin_ub; deg >= 0; --deg) {
		num_[u] += cnt_[deg];
		if (num_[u] >= deg) {
			return deg;
		}
	}
}

void core_decomposition() {
	bool update;
	do {
// init_time("one round");
		update = false;
		#pragma omp parallel
		{
			#pragma omp for reduction(| : update)
			for (int u = 0;  u < N; ++u) {
				if (num_[u] >= ub[u]) continue;

				cur_ub[u] = get_estimate(u, ub[u]);
				ub_update[u] = true;
				update |= true;
			}
			
			#pragma omp for
			for (int u = 0;  u < N; ++u) {
				if (!ub_update[u]) continue;
				ub_update[u] = false;

				for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
					const int &v = edges[e_idx];

					if (cur_ub[v]>cur_ub[u] && cur_ub[v]<=ub[u] && num_[v]>=cur_ub[v]) {
						omp_set_lock(&lock_[v]);
						// #pragma omp atomic
						--num_[v];
						omp_unset_lock(&lock_[v]);
					}
				}
				ub[u] = cur_ub[u];
			}
			// #pragma omp for	schedule(dynamic, 128)
			// for (int u = 0;  u < N; ++u) {
			// 	for (long long e_idx = pos[u]; e_idx < pos[u+1]; ++e_idx) {
			// 		const int &v = edges[e_idx];
			// 		if (!ub_update[v]) continue;
			// 		if (cur_ub[v]<cur_ub[u] && ub[v]>=cur_ub[u]) {
			// 			--num_[u];
			// 		}
			// 		if (num_[u] < cur_ub[u]) break;
			// 	}
			// }
			// #pragma omp for
			// for (int u = 0; u < N; ++u) {
			// 	if (ub[u] != cur_ub[u]) ub[u] = cur_ub[u];
			// 	ub_update[u] = false;
			// }
		}
// get_time_cost("one round");
	} while (update);
	
}

void printCore() {
	for (int i = 0; i < N; ++i) {
		printf("%d ", ub[i]);
	}
	puts("");
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

	// Create Graph
	int max_deg = 0;
	N = g_.links.size();
	
	pos = new long long[N + 1];
    long long _cnt = 0;
    for (int i = 0; i < N; ++i) {
        pos[i] = _cnt;
        _cnt += g_.links[i].size();

		if (max_deg < g_.links[i].size())
			max_deg = g_.links[i].size();
    }
    pos[N] = _cnt;

    edges = new int[_cnt];
    _cnt = 0;
    for (auto nbr : g_.links) {
        for (auto v : nbr) {
            edges[_cnt++] = v;
        }
    }

	// Init
	omp_set_num_threads(32);

	ub = new int[N];
	cur_ub = new int[N];
	cnt_ = new int[max_deg+1];
	
	num_ = new int[N];
	ub_update = new bool[N];
	lock_ = new omp_lock_t[N];

	#pragma omp parallel for
	for (int i = 0; i < N; ++i) {
		cur_ub[i] = ub[i] = g_.links[i].size();

		num_[i] = 0;
		ub_update[i] = false;
		omp_init_lock(&lock_[i]);
	}
    // Algo
init_time("algo");
    core_decomposition();
get_time_cost("algo");

#ifdef DEBUG
    printCore();
#endif

	#pragma omp parallel for
	for (int i = 0; i < N; ++i) {
		omp_destroy_lock(&lock_[i]);
	}
	delete []ub;
	delete []cur_ub;
	delete []cnt_;
	delete []num_;
	delete []ub_update;
	delete []lock_;
    return 0;
}
/*void generate_core() {
	const int &n = g_->vertices_num_;

	memset(bin_, 0, sizeof(int) * (g_->max_deg_+2));
	
	for (int v = 0; v < n; ++v) {	//O(V) create d
		degree_tag_[v] = g_->vertices_[v]->degree;
		bin_[degree_tag_[v]]++;
	}
	
	int start = 0;
	for (int d = 0; d <= g_->max_deg_; ++d) {	//O(MAX_DEG) create b
		int num = bin_[d];
		bin_[d] = start;
		start += num;
	}
	
	//bin-sort vertices by degree
	for (int v = 0; v < n; ++v) {	//O(V) create p and D
		pos_[v] = bin_[degree_tag_[v]];
		core_[pos_[v]] = v;
		bin_[degree_tag_[v]]++;
	}
	//recover bin[]
	for (int d = g_->max_deg_; d >= 1; --d) {
		bin_[d] = bin_[d-1];
	}
	bin_[0] = 0;
	bin_[g_->max_deg_+1] = g_->vertices_num_;

	//main algorithm
	for (int i = 0; i < n; ++i) {	//O(E) delete all Nodes from D[]
		int v = core_[i];
		Vertex* cur_vert = g_->vertices_[v];
		for (auto u : cur_vert->nbr) {
			if (degree_tag_[u] > degree_tag_[v]) {
				int du = degree_tag_[u], pu = pos_[u];
				int pw = bin_[du], w = core_[pw];
				if (u != w) {
					pos_[u] = pw; core_[pu] = w;
					pos_[w] = pu; core_[pw] = u;
				}
				bin_[du]++;
				degree_tag_[u]--;
			}
		}
	}
}*/
