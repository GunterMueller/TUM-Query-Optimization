#include "JoinTree.hpp"
#include "QueryGraph.hpp"

JoinTree::JoinTree(){

}

//"size" of the tree: number of nodes
int JoinTree::size(){
    if(isLeaf) {
        return 1;
    }
    return rightSub->size() + leftSub->size();
}

//recursively calculate cardinality
double JoinTree::cardinality(const QueryGraph& graph) const {
    if (isLeaf) {
        return node->cardinality_;
    } else {
        return cardinality(graph, *leftSub, *rightSub);
    }
}

std::vector<QueryGraphNode> JoinTree::get_tree_relations(const JoinTree& tree) {
    std::vector<QueryGraphNode> nodes;
    std::stack<const JoinTree*> trees{{&tree}};
    while (!trees.empty()) {
        const JoinTree* tree = trees.top();
        trees.pop();
        if (tree->isLeaf) {
            nodes.emplace_back(*tree->node);
        } else {
            trees.push(tree->leftSub.get());
            trees.push(tree->rightSub.get());
        }
    }
    return nodes;
}


double JoinTree::cardinality(const QueryGraph& graph, const JoinTree& left, const JoinTree& right) {
    const auto relations_left = get_tree_relations(left);
    const auto relations_right = get_tree_relations(right);
    double cardinality = left.cardinality(graph) * right.cardinality(graph);
    for (const auto& node : relations_left) {
        const auto& edges = graph.at(node.relation_.binding).second;
        for (const auto& edge : edges) {
            if (
                    std::find(
                        relations_right.begin(),
                        relations_right.end(),
                        edge.connected_to_
                    ) != relations_right.end()
            ) {
                cardinality *= edge.selectivity_;
            }
        }
    }
    return cardinality;
}

//Recursively calculate cost
double JoinTree::cost(const QueryGraph& graph) const {
    if (isLeaf) {
        return 0;
    } else {
        return cardinality(graph) + leftSub->cost(graph) + rightSub->cost(graph);
    }
}

double JoinTree::cost(const QueryGraph& graph, const JoinTree& left, const JoinTree& right) {
    return cardinality(graph, left, right) + left.cost(graph) + right.cost(graph);
}