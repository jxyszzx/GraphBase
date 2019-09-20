#include "graph.h"
#include "lpa.h"

using namespace std;

int main(int argc, char *argv[]) {
    string filename(argv[1]);
    filename = "../datasets/" + filename;
    int times = atoi(argv[2]);

    LPA lpa(true, filename);
    lpa.g.display();
    lpa.algo(times);
    lpa.display();
    return 0;
}