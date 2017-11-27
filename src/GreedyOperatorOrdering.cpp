#include "GreedyOperatorOrdering.hpp"

JoinTree run(const QueryGraph& graph) {

    //First create the tree with disconnected leafs from the set of relations
    //The set of nodes to begin with
    std::list<JoinTree> tree;

    //for every node in the graph, create node (relation) in tree
    for(auto node : graph) {
        //add to "tree" list
        //node is map, second is pair, first is the node object
       tree.emplace_back(node.second.first);
    }
    
    //while there are still disconnected nodes or sub-trees, run the algo from the lecture
    while(tree.size() > 1) {
        //Find minimal pair to join
        //Pointer to sub tree one
        auto leftSub = tree.begin();
        //Pointer to sub tree two
        auto rightSub = tree.end();
        rightSub--;

        //Start with infinity
        double minimum = std::numeric_limits<double>::infinity();

        //start to iterate, compare in inner loop with every other possibility
        auto start_it = tree.begin();
        while(start_it != tree.end()) {
            //Comparison
            auto end_it = start_it;
            end_it++;
            while(end_it != tree.end()) {
                //Get the cost of this proposed join
                double cost = JoinTree::cost(graph, *start_it, *end_it);
                //If cost is smaller than what we have, replace external pointers
                if(cost < minimum) {
                    minimum = cost;
                    //Remember the proposed join pair
                    leftSub = start_it;
                    rightSub = end_it;
                    std::cout << "Chose at cost: " << std::to_string(cost) << "\n";
                }

                end_it++;
            }

            start_it++;
        }

        //We should now have the minimum, create tree from sub trees
        JoinTree joined(std::move(*leftSub), std::move(*rightSub));
        //Delete from our working set and insert the new tree
        tree.erase(leftSub);
        tree.erase(rightSub);
        tree.push_back(joined);

    }
    
    return tree.front();
}

