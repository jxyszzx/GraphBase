 ///
 /// @file    :main.cpp
 /// @author  :wmjtxt(972213032@qq.com)
 /// @date    :2018-11-11 21:44:15(NewYork time)
 /// @quote   :
 ///
 
#include "lpa.h"

using namespace std;


void display_time(const char *str){
	time_t rawtime;
	time ( &rawtime );
	cerr << str << " : " << ctime (&rawtime);
}

int main(int argc, char **argv){
	srand(time(NULL));

	//parse_args(argc, argv);
	if(argc != 2){
		cerr << "argc not 2" << endl;
	}
	string filename = argv[1];
	cout << "filename = " << filename << endl;
	
	time_t time_begin, time_end;
	time(&time_begin);
	display_time("start");

	//sleep(3);
	LPA lpa(filename);
	double modu = lpa.modularity();
	double maxQ = modu;
	cout << "初始模块度 = " << modu << endl;
	for(int i = 0; i < 5; i++){
		modu = lpa.modularity();
		lpa.labeled();
		lpa.display();
		//lpa.displayLabels();
		if(maxQ < modu){
			maxQ = modu;
			lpa.getmaxComms();
		}
		cout << "迭代次数：" << i+1 << endl;
		if(modu - lpa.modularity() > 0.5)
			break;
	}

	//打印最后结果
	lpa.displayResult();
	//lpa.displayLabels();

	display_time("end");

	time(&time_end);
	return 0;
}
