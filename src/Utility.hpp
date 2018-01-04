#ifndef H_Utility
#define H_Utility

#include "QueryGraph.hpp"
#include "JoinTree.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <list>

namespace utility {
    void printGraph(QueryGraph& graph);
    void subset(std::vector<int> arr, int size, int left, int index, std::list<int> &l, std::set<std::set<int> >& akku);
    std::set<std::set<int> > subsets(std::vector<int> arr, int size, int left, int index, std::list<int> &l);
    bool setIntersect(std::set<int> A, std::set<int> B);
    void treesFromNodes(std::vector<JoinTree>& trees,QueryGraph& graph);
}

#endif