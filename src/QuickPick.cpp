#include "QuickPick.hpp"

using namespace std;

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
    std::vector<unique_ptr<JoinTree>> trees;

    for(auto& x : graph) {
        cout << "Created tree for: " << x.second.first.relation_.name << endl;
        trees.push_back(make_unique<JoinTree>(JoinTree(x.second.first)));
    }

    vector<pair<QueryGraphNode*,QueryGraphEdge*>> edges;

    
    for(auto entry : graph) {
        auto node = entry.second.first;
        for(auto edge : entry.second.second) {
            edges.emplace_back(&node,&edge);
        }
    }

    auto test = trees.begin();
    while(test != trees.end()) {
        cout << test->get()->node->relation_.name << endl;
        test++;
    }

    cout << "Created edge vector" << endl;

    std::srand(std::time(nullptr)); // use current time as seed for random generator

    //Loop until only one tree is left: The result

    while(trees.size() > 1) {

        int randomInt = 0;

        //Select random element;
        randomInt = (rand() % static_cast<int>(edges.size()));
        cout << "Taking random edge: " << randomInt << endl;

        //We take a random edge
        auto edge = &edges[randomInt];
        
        //Check if "edge" connects two relations, which are in different subtrees
        auto endIt = trees.begin();
        while(endIt != trees.end()) {
            //TODO: Check this
              if(endIt->get()->node->relation_.getName() == edge->second->connected_to_.relation_.getName()) {
                //edge is connected to rel in t
                for(auto startIt = trees.begin();startIt != trees.end();startIt++) {
                    if(startIt->get()->node->relation_.getName() == edge->first->relation_.getName()) {
                        //Edge begins at start
                        //now check if tree is the same, by comparing ref
                        if(!(&(*startIt) == &(*endIt))) {
                            cout << "Found edge with relations in diff. trees" << endl;
                            //pointing not to same memory -> diff. trees
                            //Join their trees
                            //trees.push_back(make_unique<JoinTree>(JoinTree(move(*(startIt->get())),move(*(endIt->get())))));
                            //Remove both from vector
                            startIt = trees.erase(startIt);
                            if(startIt == trees.end())
                                break;
                            endIt = trees.erase(endIt);
                            if(endIt == trees.end())
                                break;
                            trees.shrink_to_fit();
                        }
                    }
                }
            }
            endIt++;
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
    
    return *trees[0].get();
}