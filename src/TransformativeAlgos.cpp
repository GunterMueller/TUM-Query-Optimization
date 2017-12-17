#include "TransformativeAlgos.hpp"

using namespace std;

uint64_t generateClassId(vector<int> relationIDs) {
    uint64_t id = 0;

    for(int i : relationIDs) {
        id = (id << 32 | i);
    }

    return id;
}

//Transformation rules
//Modify directly
//Commutativity: swap left and right sub tree of the join
void commutativity(JoinTree& join) {
    if(join.commDisabled)
        return;
    //The swap
    join.leftSub.swap(join.rightSub);
    //To reduce the number of duplicates, disable T1-T3 for new tree
    //TODO: Disable transformations
    join.commDisabled = true;
    join.rightDisabled = true;
    join.leftDisabled = true;
}

//Right associativity: requires on level more
void rightAssociativity(JoinTree& join) {
    if(join.rightDisabled)
        return;
    //Swap in three steps
    join.leftSub.swap(join.rightSub);
    join.leftSub.swap(join.rightSub->leftSub);
    join.rightSub->leftSub.swap(join.rightSub->rightSub);

    //TODO: Disable/enable transformations
    join.leftDisabled = true;
    join.rightDisabled = true;

    join.rightSub->commDisabled = false;
    join.rightSub->leftDisabled = false;
    join.rightSub->rightDisabled = false;
}

//Left associativity: basically the same, just the other way around
void leftAssociativity(JoinTree& join){
    if(join.leftDisabled)
        return;
    //Swap
    join.leftSub.swap(join.rightSub);
    join.rightSub.swap(join.leftSub->rightSub);
    join.leftSub->leftSub.swap(join.leftSub->rightSub);

    //TODO: Disable/enable transformations
    join.leftDisabled = true;
    join.rightDisabled = true;

    join.leftSub->commDisabled = false;
    join.leftSub->leftDisabled = false;
    join.leftSub->rightDisabled = false;
}



/** Implementation of the tranformative algorithm
 * that makes use of the memo structure
 **/
JoinTree TransformativeAlgos::exhaustiveTrans2(QueryGraph graph, int numberOfRelations) {
    cout << "Starting exhaustive transformation" << endl;
    //1. Initialize MEMO
    cout << "Initializing memo structure " << endl;

    int n = numberOfRelations;

    vector<int> start(n);
    for (int i = 1; i <= n; i++)
    {
        start[i] = i;
    }
    list<int> lt;

    //Generate subsets
    auto subsets = utility::subsets(start,n,1,0,lt);

    for(int i = 2; i<=n;i++) {
        auto s2 = utility::subsets(start,n,i,0,lt);
        subsets.insert(s2.begin(),s2.end());
    }
    //Done

  
    for (auto s : subsets) {
        cout << "{";
        vector<int> v;
        for(int i : s) {
            cout << i+1;
            v.push_back(i+1);
        }
        //Generate the id for the memo class
        uint64_t id = generateClassId(v);
        //Create entry in memo table
        vector<JoinTree> vec;
        if(s.size() == 1) {
            for(auto x : graph) {
                if(x.second.first.relation_.id == *(s.begin()))
                    vec.push_back(JoinTree(x.second.first));
            }          
        } else {
            auto it = s.begin();
            vector<unique_ptr<JoinTree>> intermediate;
            while(it != s.end()) {
                for (auto x : graph) {
                    if(x.second.first.relation_.id == *it)
                        intermediate.push_back(make_unique<JoinTree>(JoinTree(x.second.first)));
                }
                it++;
            }

            //Combine until one left
            while(intermediate.size() >= 2) {
                //cout <<  endl << "Combining at size: " << intermediate.size() << endl;
                auto it1 = intermediate.begin();
                auto it2 = it1 + 1;
                auto t1 = move(*it1);
                auto t2 = move(*it2);
                intermediate.erase(it1);
                intermediate.erase(it1);
                intermediate.push_back(make_unique<JoinTree>(JoinTree(move(*t1.release()),move(*t2.release()))));
                intermediate.shrink_to_fit();
            }

            //Combination done
            //cout << endl << "Combinaton done " << endl;
            vec.push_back(move(*(*intermediate.begin()).get()));
        }
        memo.insert({{id,vec}});
        cout << "}" << endl;
    }

    //2. ExploreClass
    vector<int> allRel(n);
    iota(allRel.begin(), allRel.end(), 1);
    exploreClass(allRel);


    //3. return minimal join tree from the class under some cost function
    auto jts = memo.find(generateClassId(allRel))->second;
    auto minimal = jts.begin();
    for(auto it = jts.begin();it!=jts.end();it++){
        if(it->cost(graph) < minimal->cost(graph)) {
            minimal = it;
        }
    }
    return *minimal;
}

/**
 * The exploration method
 * Considers *all* alternatives on one class and applies transformation
**/
void TransformativeAlgos::exploreClass(vector<int> relSetId) {

    cout << "Exploring class: " << endl;
    cout << "{";
    for (int i : relSetId) {
        cout << i << " ";
    }
    cout << "}" << endl;
    
    //Find class of the input
    auto jts = memo.find(generateClassId(relSetId));
    if(jts == memo.end()) {
        return;
    }
    auto theClass = jts->second;

    //while not all trees of C have been explored
    for(auto tree : theClass) {
        //1. choose some unexplored tree
        if(tree.explored == false) {
            //2. ApplyTransformation2
            cout << "Applying trans" << endl;
            applyTrans(tree);
            //3. Mark T as explored
            tree.explored = true;
        }
    }
}
/**
 * Actual application of the transformations
 * Here: RuleSet RS-1 from the lecture
 **/
void TransformativeAlgos::applyTrans(JoinTree theTree) {

    //1. Before doing anything, descend into child trees
    //exploreClass(theTree.leftSub);
    if(theTree.leftSub) {
        auto rels1 = JoinTree::get_tree_relations(*theTree.leftSub);
        vector<int> rel1IDs;
        for (auto q : rels1){
            rel1IDs.push_back(q.relation_.id);
        }
        exploreClass(rel1IDs); 
    }

    //exploreClass(theTree.rightSub);
    if(theTree.rightSub) {
        auto rels2 = JoinTree::get_tree_relations(*theTree.rightSub);
        vector<int> rel2IDs;
        for (auto q : rels2){
            rel2IDs.push_back(q.relation_.id);
        }
        exploreClass(rel2IDs);
    }

    //2. for every rule and for every member of child "classes"
    auto rels = JoinTree::get_tree_relations(theTree);
    vector<int> relIDs;
    for (auto q : rels){
        relIDs.push_back(q.relation_.id);
    }
    auto searchResult = memo.find(generateClassId(relIDs));
    if(searchResult != memo.end()) {
        for(auto t : searchResult->second) {
            commutativity(t);
            JoinTree res1 = JoinTree(t);
            searchResult->second.push_back(res1);

            leftAssociativity(t);
            JoinTree res2 = JoinTree(t);
            searchResult->second.push_back(res2);

            rightAssociativity(t);
            JoinTree res3 = JoinTree(t);
            searchResult->second.push_back(res3);
        }
    }
}