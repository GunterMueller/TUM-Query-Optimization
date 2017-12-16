#ifndef H_DPAlgos
#define H_DPAlgos

#include <map>
#include <utility>
#include "JoinTree.hpp"
#include "QueryGraph.hpp"
#include "cts/parser/SQLParser.hpp"
#include "Utility.hpp"

class DPAlgos {

    public:
    std::map<std::string,JoinTree> DPsize(QueryGraph graph);
    JoinTree CreateJoinTree(JoinTree&& left, JoinTree&& right);

};

#endif