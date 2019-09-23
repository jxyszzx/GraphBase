#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <set>
#include <omp.h>
#include "graph.h"
using namespace std;

typedef vector<int> VI;
typedef vector<VI> VVI;

const int maxn = 1e5+5;

bool vis[maxn], updated;
vector<int> cc_label;
Graph g;

void do_wcc() {
    #pragma omp parallel for
    for (int i = 0; i < g.links.size(); ++i) {
        if (vis[i]) continue;
        const VI& nbrs = g.links[i];
        for (auto nbr : nbrs) {
            #pragma omp critical
                cc_label[nbr] = min(cc_label[nbr], g.label[i]);
        }
    }
    #pragma omp parallel for
    for (int i = 0; i < cc_label.size(); ++i) {
        if (cc_label[i] == g.label[i]) {
            vis[i] = true;
        } else {
            g.label[i] = cc_label[i];
            vis[i] = false;
            updated = true;
        }
    }
}

void printCC() {
    set<int> comms;
    for (auto label: g.label) {
        comms.insert(label);
    }

    printf("社团个数 : %lu = { ", comms.size());
    for (auto c : comms) {
        printf("%d ", c);
    }
    puts("}");

    for (int i = 0; i < g.label.size(); ++i) {
        printf("[%d]%s", g.label[i], i+1==g.label.size() ? " }\n" : ",");
    }
}

int main(int argc, char *argv[])
{
    string filename(argv[1]);
    filename = "../datasets/" + filename;
    g = Graph(true, filename);

    // Init
    cc_label.assign(g.label.begin(), g.label.end());
    memset(vis, 0, sizeof vis);
    // Algo
    do {
        updated = false;
        do_wcc();
    } while (updated);

    printCC();
    return 0;
}