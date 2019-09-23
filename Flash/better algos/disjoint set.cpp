#include <iostream>
#include <cstring>
#include <vector>
#include "omp.h"
using namespace std;

const int maxn = 1e5+5;

int n, m;
int f[maxn];
vector<pair<int, int> > edges;

int Find(int x) {
    vector<int> path;
    while (x != f[x]) {
        path.push_back(x);
        x = f[x];
    }
    #pragma omp parallel for
    for (int i = 0; i < path.size(); ++i) {
        f[path[i]] = x;
    }
}

void Union(int u, int v) {
    if (Find(u) != Find(v)) {
        f[u] = f[v];
    }
}

int main()
{
    cin >> n >> m;
    int u, v;
    while (cin >> u >> v) {
        edges.push_back(make_pair(u, v));
    }

    // init
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        f[i] = i;
    }

	// algo
    for (auto edge : edges) {
        Union(edge.first, edge.second);
    }

    // print
	for (int i = 0; i < n; ++i) {
		printf("Vertex %d belongs to %d\n", i, f[i]);
	}
	return 0;
}