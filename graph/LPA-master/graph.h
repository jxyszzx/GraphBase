 ///
 /// @file    :graph.h
 /// @author  :wmjtxt(972213032@qq.com)
 /// @date    :2018-11-11 20:52:12(NewYork time)
 /// @quote   :
 ///


#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm> 

using namespace std; 

class Graph{
public:
	vector<vector<int> > links;//邻接表
	vector<int> label;//标签
	Graph();
	Graph(string);
	void display();

	int nb_nodes;
	int nb_links;

	inline int nb_selfloops(int comm);
	inline int weighted_degree(int comm);
};

inline int Graph::nb_selfloops(int comm){
	assert(comm >= 0 && comm < nb_nodes);
	
	int res = 0;
	set<int> commi;
	vector<int> nodes;
	for(int i = 0; i < nb_nodes; i++){
		if(label[i] == comm){
			commi.insert(i);
			nodes.push_back(i);
		}
	}
	for(auto & item : commi){
		for(unsigned j = 0; j < links[item].size(); j++){
			if(commi.find(links[item][j]) != commi.end()){
				res++;
			}
		}
	}
	//这里没除2，因为后面代入公式的时候，除的是2M，正好抵消 
	//一开始我除2了，模块度一直0.0几，害我找了好久
	return res;
}

inline int Graph::weighted_degree(int comm){
	assert(comm >= 0 && comm < nb_nodes);
	
	int res = 0;
	set<int> commi;
	for(int i = 0; i < nb_nodes; i++){
		if(label[i] == comm){
			commi.insert(i);
		}
	}
	for(auto & item : commi){
		res += links[item].size();
	}
	return res;
}

#endif
