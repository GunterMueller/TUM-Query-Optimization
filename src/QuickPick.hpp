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
    QuickPick(std::vector<JoinTree>& trees, QueryGraph& graph);
    JoinTree QP();
};

#endif