#ifndef H_QuickPick
#define H_QuickPick

#include <vector>
#include <cstdlib>
#include <ctime>

#include "JoinTree.hpp"
#include "QueryGraph.hpp"
#include "Utility.hpp"

class QuickPick {
    public:
    QuickPick(std::vector<JoinTree>& trees, QueryGraph& graph) : trees{trees}, graph{graph} {}
    std::unique_ptr<JoinTree> QP();
    std::vector<JoinTree>& trees;
    QueryGraph& graph;
};

#endif