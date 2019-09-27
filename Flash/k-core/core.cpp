#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "graph.h"
#include "omp.h"

// #define DEBUG

using namespace std;

Graph g_;
int max_deg, n;
vector<int> ub;

void core_decomposition() {
	vector<int> cnt(n, 0);
	ub.assign(n, 0);

	// #pragma omp parallel for
	for (int i = 0; i < n; ++i) {
		ub[i] = g_.links[i].size();
	}

	bool update;
	do {
		update = false;
		#pragma omp parallel for
		for (int u = 0;  u < n; ++u) {
			if (cnt[u] >= ub[u]) {
				continue;
			}
			int originUb = ub[u];

			vector<int> bin(n, 0);
			for (auto v : g_.links[u]) {
				++bin[min(ub[u], ub[v])];
			}

			cnt[u] = 0;
			for (int deg = originUb; deg > 0; --deg) {
				cnt[u] += bin[deg];
				if (cnt[u] >= deg) {
					ub[u] = deg;
					break;
				}
			}
			
			if(ub[u] < originUb){
				update = true;
				for (auto v : g_.links[u]) {
					if(ub[v] > ub[u] && ub[v] <= originUb && cnt[v] >= ub[v]){
						--cnt[v];
					}
				}
			}
		}
	} while (update);
}

void printCore() {
	for (auto core : ub) {
		printf("%d ", core);
	}
	puts("");
}

int main(int argc, char *argv[])
{
    string filename(argv[1]);
    filename = "../datasets/" + filename;
    g_ = Graph(true, filename);

	// Init
	n = g_.links.size();
	max_deg = 0;
	for (auto vi : g_.links) {
		if (max_deg < vi.size()) {
			max_deg = vi.size();
		}
	}
    // Algo
    core_decomposition();

#ifdef DEBUG
    printCore();
#endif

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
