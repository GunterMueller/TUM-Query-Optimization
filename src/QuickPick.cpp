#include "QuickPick.hpp"

using namespace std;

/**
 * Helper methods
 **/ 
unique_ptr<JoinTree> findTreeForRelation(vector<unique_ptr<JoinTree>>& trees, const SQLParser::Relation& relation) {
    //Find tree that contains relation
    for(auto treeIt = trees.begin(); treeIt != trees.end(); treeIt++) {
        if(JoinTree::containsRelation(relation,*treeIt->get()))
        {
            //We move out of the vector, don't forget to clean up
            auto localptr = move(*treeIt);
            trees.erase(treeIt);
            return localptr;
        } else {
            return unique_ptr<JoinTree>(nullptr);
        }
    }
}

/**
 * Implements the QuickPick Algorithm from the lecture
 * which takes a random edge in the query graph
 * connecting two relations in different subtrees and
 * joins these trees. This is repeated until all subtrees
 * are joinend and the result tree is left. The algorithm 
 * is usually run multiple times to find a good tree
 **/
JoinTree QuickPick::QP(QueryGraph graph) {

    // The initial set of trees are just nodes of relations
    vector<unique_ptr<JoinTree>> trees;
    utility::treesFromNodes(trees,graph);

    vector<pair<QueryGraphNode*,QueryGraphNode*>> edges;
    
    //Create list of edges from the query graph
    for(auto entry : graph) {
        auto node = entry.second.first;
        for(auto edge : entry.second.second) {
            edges.emplace_back(&node,&edge.connected_to_);
        }
    }
    cout << "Created edge vector" << endl;

    // use current time as seed for random generator
    std::srand(std::time(nullptr)); 

    //Loop until only one tree is left: The result
    while(trees.size() > 1) {

        int randomInt = 0;

        //Select random element;
        randomInt = (rand() % static_cast<int>(edges.size()));
        cout << "Taking random edge: " << randomInt << endl;

        //We take a random edge
        auto edge = &edges[randomInt];
        
        //Check if the edge connects two relations, which are in different subtrees
        SQLParser::Relation& startRel = edge->first->relation_;
        SQLParser::Relation& endRel = edge->second->relation_;
        
        //Note that this "remove" the tree from further consideration
        unique_ptr<JoinTree> startRelTree = findTreeForRelation(trees,startRel);
        //In the remaining trees, we look for a matching tree for the second relation
        unique_ptr<JoinTree> endRelTree = findTreeForRelation(trees,endRel);

        //If we don't find one, the two relations are not in different trees, if we find one, it cannot be one we already
        //found, as we created one tree for every node in the query graph and because of the unique pointers.
        if(endRelTree) {
            cout << "Relations are in different trees!" << endl;
            //Join the trees and push result to vector
            trees.push_back(make_unique<JoinTree>(JoinTree(move(*startRelTree.release()),move(*endRelTree.release()))));
        } else {
            //Put back the trees, maybe another edge will consume them
            if(startRelTree)
                trees.push_back(move(startRelTree));
        }

        //Remove used edge
        edges.erase(edges.begin() + randomInt);
        edges.shrink_to_fit();
        cout << "Edges left: " << edges.size() << endl;
        if(edges.size() == 0) {
            //No edges left, theres is nothing we can do anymore
            break;
        }

        cout << "Trees left: " << trees.size() << endl;
        
    }
    cout << "Only one tree left or no more edges" << endl;
    return *trees.back().get();
}