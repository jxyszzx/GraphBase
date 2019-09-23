#include "lpa.h"
#include <set>
using namespace std;

LPA::LPA(bool is_undirected, string filename) {
    g = Graph(is_undirected, filename);
}

void
LPA::algo(int times) {
    while (times--) {
        if(!label()) {
            break;
        }
    }
    getComms();
}

void
LPA::getComms() {
    set<int> diff_label;
    for (int i = 0; i < g.label.size(); ++i) {
        diff_label.insert(g.label[i]);
    }

    comms.clear();
    for (auto label : diff_label) {
        comms.push_back(label);
    }
}

bool
LPA::label() {
    VI new_label(g.links.size());
    #pragma omp parallel for
    for (int i = 0; i < g.links.size(); ++i) {
        const VI& nbrs = g.links[i];
        VI cnt(g.links.size(), 0);
        for (int j = 0; j < nbrs.size(); ++j) {
            ++cnt[g.label[nbrs[j]]];
        }
        
        int max_label = 0, max_num = 0;
        for (int j = 0; j < cnt.size(); ++j) {
            if (cnt[j] > max_num) {
                max_label = j;
                max_num = cnt[j];
            }
        }
        new_label[i] = max_label;
    }

    bool is_updated = false;
    #pragma omp parallel for
    for (int i = 0; i < g.links.size(); ++i) {
        if (g.label[i] != new_label[i]) {
            g.label[i] = new_label[i];
            is_updated = true;
        }
    }
    return is_updated;
}

void
LPA::display() {
    printf("社团个数 : %d = { ", comms.size());
    for (int i = 0; i < comms.size(); ++i) {
        printf("[%d]%s", comms[i], i+1==comms.size() ? " }\n" : ",");
    }
    for (int i = 0; i < g.label.size(); ++i) {
        printf("[%d]%s", g.label[i], i+1==g.label.size() ? " }\n" : ",");
    }
}