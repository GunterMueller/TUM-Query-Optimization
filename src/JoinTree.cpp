#include "JoinTree.hpp"
#include "QueryGraph.hpp"

// Greedy Operator Ordering
double JoinTree::cost(const QueryGraph& graph, const JoinTree& left, const JoinTree& right) {
    return cardinality(graph, left, right) + left.cost(graph) + right.cost(graph);
}