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
        } 
    }
    return unique_ptr<JoinTree>(nullptr);
}
/**
 * Implements the QuickPick Algorithm from the lecture
 * which takes a random edge in the query graph
 * connecting two relations in different subtrees and
 * joins these trees. This is repeated until all subtrees
 * are joinend and the result tree is left. The algorithm 
 * is usually run multiple times to find a good tree
 **/

unique_ptr<JoinTree> QuickPick::QP() {

    //Keep track of edges
    vector<pair<QueryGraphNode*,QueryGraphNode*>> edges;
    
    //Create list of edges from the query graph
    for(auto entry : graph) {
        auto node = entry.second.first;
        for(auto edge : entry.second.second) {
            edges.emplace_back(&node,&edge.connected_to_);
        }
    }
    cout << "Created edge vector" << endl;

    //Create pointers for use in the algorithm
    vector<unique_ptr<JoinTree>> treePtrs;
    for (auto tree : trees) {
        treePtrs.push_back(make_unique<JoinTree>(tree));
    }

    // use current time as seed for random generator

    //Loop until only one tree is left: The result
    while(treePtrs.size() > 1) {
        
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
        //TODO: Check result of func
        unique_ptr<JoinTree> startRelTree = findTreeForRelation(treePtrs,startRel);
        //In the remaining trees, we look for a matching tree for the second relation
        unique_ptr<JoinTree> endRelTree = findTreeForRelation(treePtrs,endRel);

        if(startRelTree)
        {
            if(endRelTree) 
            {
                cout << "Relations are in different trees!" << endl;
                //Join the trees and push result to vector
                JoinTree t = JoinTree(move(*startRelTree.get()),move(*endRelTree.get()));
                trees.push_back(t);
                treePtrs.push_back(make_unique<JoinTree>(t));
            }   
            else
            {
                //Put back the trees, maybe another edge will consume them
                treePtrs.push_back(move(startRelTree));
            }
        }

        //Remove used edge
        edges.erase(edges.begin() + randomInt);
        edges.shrink_to_fit();
        cout << "Edges left: " << edges.size() << endl;
        if(edges.size() == 0) {
            //No edges left, theres is nothing we can do anymore
            break;
        }

        cout << "Trees left: " << treePtrs.size() << endl;
    }
    cout << "Only one tree left or no more edges" << endl;
    return move(treePtrs.back());
}
