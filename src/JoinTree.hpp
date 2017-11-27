#ifndef H_JoinTree
#define H_JoinTree

#include "QueryGraph.hpp"
#include <memory>
#include <utility>
#include <stack>

class JoinTree {


    //Constructors
    public:
        JoinTree();
        //Create from node, that's a leaf
        JoinTree(QueryGraphNode& node) : isLeaf{true}, node{&node}, leftSub{}, rightSub{} {}

        //Create from two sub trees (with move semantic)
        JoinTree(JoinTree&& left, JoinTree&& right) : isLeaf{false}, leftSub{std::make_unique<JoinTree>(left)}, rightSub{std::make_unique<JoinTree>(right)} {}

        //For creating from reference (move semantic and by ref)
        JoinTree(JoinTree&& other) : isLeaf(other.isLeaf), node{other.node},leftSub{std::move(other.leftSub)},rightSub{std::move(other.rightSub)} {}
        JoinTree(const JoinTree& other): isLeaf(other.isLeaf), node{nullptr}, leftSub{}, rightSub{} {
        if (isLeaf) {
            node = other.node;
        } else {
            leftSub = std::make_unique<JoinTree>(*other.leftSub);
            rightSub = std::make_unique<JoinTree>(*other.rightSub);
        }
    }
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
        double cost(const QueryGraph& graph) const;
        static double cost(const QueryGraph& graph, const JoinTree& left, const JoinTree& right);
        double cardinality(const QueryGraph& graph) const;
        static double cardinality(const QueryGraph& graph, const JoinTree& left, const JoinTree& right);
        static std::vector<QueryGraphNode> get_tree_relations(const JoinTree& tree);
};

#endif