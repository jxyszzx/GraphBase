# GraphBase
This is a repository to maintain some representative graph algorithms

## Algorithms
### Label Propagation Algorithm
* location
  * You can find it in Flash/lpa
* how to use it
  * compile
    * `make`
  * run
    * `./main sample.txt 100`
    * where sample.txt is in Flash/datasets, and 100 is the cycles for the lpa
### algos in pregel-like style
* algo_list
  * [finished]wcc
  * [finished]pagerank
  * [finished]sssp
  * scc
  * mst
* location
  * You can find it in Flash/algos as pregel
* how to use it
  * compile
    * 'make'
* run
  * `./pagerank pagerank_sample.txt`
    * where pagerank_sample.txt is in Flash/datasets
### The better algos
* algo_list
  * [finished]disjoint set for wcc
  * [finished]dijkstra for sssp
  * kosaraju for scc
