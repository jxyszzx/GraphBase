 ///
 /// @file    :lpa.h
 /// @author  :wmjtxt(972213032@qq.com)
 /// @date    :2018-11-11 20:15:42(NewYork time)
 /// @quote   :
 ///

#ifndef LPA_H
#define LPA_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "graph.h"

using namespace std;

class LPA{
public:
	Graph g;
	int size;
	int commnum;
	// used to compute the modularity
	vector<int> in,tot;
	//存放社团
	vector<int> maxComms;
	vector<int> maxLabel;
	double maxQ;
	vector<int> comms;
	LPA(string);
	void labeled();
	void display();
	void displayResult();
	void displayLabels();
	void getmaxComms();
	double modularity();
};

#endif
