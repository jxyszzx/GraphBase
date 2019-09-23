#include <iostream>
#include <cstdio>
#include <cstring>
#include <omp.h>

using namespace std;

static omp_lock_t lock;

const int maxn = 1e5+5;
int edge[maxn][maxn], dist[maxn];
bool vis[maxn];

int n, m;

void Dijkstra(int s) {
	// init
	memset(dist, 0x3f, sizeof dist);
	memset(vis, 0, sizeof vis);
	// omp_init_lock(&lock);

	// algo
	dist[s] = 0;
	for (int i = 0; i < n; ++i) {
		int min_id = 0;
		
		// omp_set_lock(&lock);
		#pragma omp parallel for
		for (int j = 0; j < n; ++j) {
			#pragma omp critical
			if (!vis[j] && dist[j] < dist[min_id]) {
				min_id = j;
			}
		}
		// omp_unset_lock(&lock);
		vis[min_id] = true;

		#pragma omp parallel for
		for (int v = 0; v < n; ++v) {
			dist[v] = min(dist[v], dist[min_id] + edge[min_id][v]);
		}
	}

	// del
	// omp_destroy_lock(&lock);
}

int main()
{
	cin >> n >> m;
	
	memset(edge, 0x3f, sizeof edge);

	for (int i = 0; i < m; ++i) {
		int u, v, w;
		cin >> u >> v >> w;
		edge[u][v] = edge[v][u] = min(edge[u][v], w);
	}

	Dijkstra(0);
	for (int i = 0; i < n; ++i) {
		printf("Path to Vertex %d is %d\n", i, dist[i]);
		// printf("%d%c", dist[i], i == n-1?'\n':' ');
	}
	return 0;
}