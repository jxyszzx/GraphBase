#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <set>
#include <omp.h>
#include "graph.h"
using namespace std;

#define DEBUG

typedef vector<int> VI;
typedef vector<VI> VVI;

const int maxn = 1e5+5;

bool vis[maxn], updated;
vector<int> dist;
Graph g;


void do_sssp() {
    #pragma omp parallel for
    for (int i = 0; i < g.links.size(); ++i) {
        if (vis[i]) continue;
        const VI& nbrs = g.links[i];
        for (auto nbr : nbrs) {
            #pragma omp critical
                dist[nbr] = min(dist[nbr], g.label[i] + 1);
        }
    }
    #pragma omp parallel for
    for (int i = 0; i < dist.size(); ++i) {
        if (dist[i] == g.label[i]) {
            vis[i] = true;
        } else {
            g.label[i] = dist[i];
            vis[i] = false;
            updated = true;
        }
    }
}

void printSSSP() {
    printf("SSSP := { ");
    for (int i = 0; i < g.label.size(); ++i) {
        if (g.label[i] == 0x3f3f3f3f) {
            g.label[i] = -1;
        }
        printf("[%d]%s", g.label[i], i+1==g.label.size() ? " }\n" : ",");
    }
}

int main(int argc, char *argv[])
{
    string filename(argv[1]);
    filename = "../datasets/" + filename;

    g = Graph(true, filename);

    // Init
    memset(vis, 0, sizeof vis);
    int source_node = 0;
    for (int i = 0; i < g.label.size(); ++i) {
        if (i == source_node) {
            g.label[i] = 0;
            continue;
        }
        g.label[i] = 0x3f3f3f3f;
    }
    dist.assign(g.label.begin(), g.label.end());
    // Algo
    do {
        updated = false;
        do_sssp();
    } while (updated);

#ifdef DEBUG
    printSSSP();
#endif

    return 0;
}