#ifndef H_JoinTree
#define H_JoinTree

#include "QueryGraph.hpp"

class JoinTree {


    //Constructors
    public:
        //Create from node, that's a leaf
        JoinTree(QueryGraphNode& node) : isLeaf{true} {}

    private:
        //Leaf flag
        bool isLeaf;
        //The node content
        QueryGraphNode* node;
        //Left
        std::unique_ptr<JoinTree> leftSub;
        //Right
        std::unique_ptr<JoinTree> rightSub;


    public:
        double cost(const QueryGraph& graph, const JoinTree& left, const JoinTree& right);
        double cardinality(const QueryGraph& graph, const JoinTree& left, const JoinTree& right);
};

#endif