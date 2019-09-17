 ///
 /// @file    :graph.cpp
 /// @author  :wmjtxt(972213032@qq.com)
 /// @date    :2018-11-11 20:55:56(NewYork time)
 /// @quote   :
 ///
 
#include "graph.h"

using namespace std;

Graph::Graph(){
	nb_nodes = 0;
	nb_links = 0;
};

Graph::Graph(string filename){
	ifstream finput;
	finput.open(filename.c_str(),fstream::in);
	nb_links = 0;
	while(!finput.eof()){
		//if(nb_links%10000 == 0) {cerr << "."; fflush(stderr);}

		unsigned int src, dest;
		finput >> src >> dest;
		//cout << src << "," << dest << endl;
		//sleep(2);
		if(finput){
			if(links.size() <= max(src,dest)+1){
				links.resize(max(src,dest)+1);
				label.resize(max(src,dest)+1);
			}
			links[src].push_back(dest);
			links[dest].push_back(src);
			label[src] = src;
			label[dest] = dest;
			nb_links++;
		}
	}
	cerr << "nb_links = " << nb_links << endl;
	nb_nodes = label.size();
	finput.close();
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
