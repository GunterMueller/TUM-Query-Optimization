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
        JoinTree(QueryGraphNode& node) : isLeaf{true}, explored{false}, commDisabled{false}, rightDisabled{false}, leftDisabled{false},node{&node}, leftSub{}, rightSub{} {}

        //Create from two sub trees (with move semantic)
        JoinTree(JoinTree&& left, JoinTree&& right) : isLeaf{false}, explored{false},commDisabled{false}, rightDisabled{false}, leftDisabled{false},leftSub{std::make_unique<JoinTree>(left)}, rightSub{std::make_unique<JoinTree>(right)} {}

        //For creating from reference (move semantic and by ref)
        JoinTree(JoinTree&& other) : isLeaf(other.isLeaf), explored{other.explored}, commDisabled{other.commDisabled}, rightDisabled{other.rightDisabled}, leftDisabled{other.leftDisabled},node{other.node},leftSub{std::move(other.leftSub)},rightSub{std::move(other.rightSub)} {}
        JoinTree(const JoinTree& other): isLeaf(other.isLeaf), explored{other.explored}, commDisabled{other.commDisabled}, rightDisabled{other.rightDisabled}, leftDisabled{other.leftDisabled}, node{nullptr}, leftSub{}, rightSub{} {
            if (isLeaf) {
                node = other.node;
            } else {
                leftSub = std::make_unique<JoinTree>(*other.leftSub);
                rightSub = std::make_unique<JoinTree>(*other.rightSub);
            }
        }
    
        //Leaf flag
        bool isLeaf;
        //Exploration Flag
        bool explored;

        //Transformation Flags
        bool commDisabled;
        bool rightDisabled;
        bool leftDisabled;

        //The node content
        QueryGraphNode* node;
        //Left
        std::unique_ptr<JoinTree> leftSub;
        //Right
        std::unique_ptr<JoinTree> rightSub;


        int size();
        double cost(const QueryGraph& graph) const;
        static double cost(const QueryGraph& graph, const JoinTree& left, const JoinTree& right);
        double cardinality(const QueryGraph& graph) const;
        static double cardinality(const QueryGraph& graph, const JoinTree& left, const JoinTree& right);
        static std::vector<QueryGraphNode> get_tree_relations(const JoinTree& tree);
};

#endif