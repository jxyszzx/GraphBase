#include "graph.h"

using namespace std;

Graph::Graph() {

}

Graph::Graph(bool is_undirected, int num, string filename) {
    cin.sync_with_stdio(false);

    switch(num) {
        case 1:
			filename = "/export/vldb/2/scratch/llai/flash_exp/datasets/com-friendster.ungraph.txt";
			break;
		case 2:
			filename = "/export/vldb/2/scratch/llai/flash_exp/datasets/eur-converted-new.txt";
			break;
		case 3:
			filename = "/export/vldb/2/scratch/llai/flash_exp/datasets/uk-2002.txt";
			break;
		case 4:
			filename = "/export/vldb/2/scratch/llai/flash_exp/datasets/com-lj.ungraph.txt";
			break;
		case 5:
			filename = "/export/vldb/2/scratch/llai/flash_exp/datasets/twitter-2010.txt";
			break;
    }
    ifstream finput(filename);
    if (!finput) {
        cerr << "Can't read the graph!\n";
    }
    // preprocessing
    if (num == 1 || num == 4) {
        string dummy_line;
        for (int i = 0; i < 4; ++i) {
            getline(finput, dummy_line);
        }
    }

    if (is_undirected) {
        build_undirected_graph(finput);
    } else {
        build_directed_graph(finput);
    }
}

inline bool
Graph::read(int& ret) {
	char ch;
	bool is_neg = false;
	while (ch = getchar()) {
		if (ch == EOF) return false;
		if (isdigit(ch) || ch == '-') {
			break;
		}
	}
	
	ret = 0;
	if (ch != '-') {
		ret = ch - '0';
	} else {
		is_neg = true;
	}
	while (isdigit(ch = getchar())) {
		ret *= 10;
		ret += ch - '0';
	}
	if (is_neg) {
		ret = -ret;
	}
	return true;
}

void
Graph::build_undirected_graph(ifstream& finput) {
    int src, dest;
    while (finput >> src) {
        finput.get();
        finput >> dest;
        if (links.size() <= max(src, dest)+1) {
            links.resize(max(src, dest)+1);
        }
        links[src].push_back(dest);
        links[dest].push_back(src);
    }
    
    label.resize(links.size());
    # pragma omp parallel for
    for (int i = 0; i < label.size(); ++i) {
        label[i] = i;
    }
}

void
Graph::build_directed_graph(ifstream& finput) {
    int src, dest;
    while (finput >> src) {
        finput.get();
        finput >> dest;
        if (links.size() <= max(src, dest)+1) {
            links.resize(max(src, dest)+1);
        }
        links[src].push_back(dest);
    }

    label.resize(links.size());
    # pragma omp parallel for
    for (int i = 0; i < label.size(); ++i) {
        label[i] = i;
    }
}

void
Graph::display(){
	cout << "Graph: " << endl;
	for(unsigned int i = 0; i < links.size(); i++){
		for(unsigned int j = 0; j < links[i].size(); j++){
			int dest = links[i][j];
			cout << i << " " << dest << endl;
		}
	}
}