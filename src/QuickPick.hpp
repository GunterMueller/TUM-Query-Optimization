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
    JoinTree QP(QueryGraph graph);
};

#endif