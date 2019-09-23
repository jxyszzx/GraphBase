#include "graph.h"

using namespace std;

Graph::Graph() {

}

Graph::Graph(bool is_undirected, string filename) {
    ifstream finput(filename);
    if (!finput) {
        cerr << "Can't read the graph!\n";
    }

    if (is_undirected) {
        build_undirected_graph(finput);
    } else {
        build_directed_graph(finput);
    }
}

void
Graph::build_undirected_graph(ifstream& finput) {
    int src, dest;
    while (finput >> src >> dest) {
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
    while (finput >> src >> dest) {
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