#include "DPAlgos.hpp"

//Joins two trees
static JoinTree CreateJoinTree(JoinTree&& left, JoinTree&& right) {
    //We currently just use what is already implemented from previous exercises
    //So no min. over join implementations
    //Call the constructor
    return JoinTree(std::move(left),std::move(right));
}

//In: A set of relations (we re-use the query graph that was generated, just query graph node objects instead of SQLParser::relation)
//Out: Optimal bushy join tree
//Note that this is not optimized in any way
static void DPsize(QueryGraph graph) {

    //Get set of nodes
    std::vector<JoinTree> R;
    for(auto kv : graph) {
        R.push_back(JoinTree(kv.second.first));
    }

    //Number of relations is vector size
    int n = R.size();

    //We just use a set of JoinTrees as the DP table, starting with single nodes
    //By increasing set size
    for(int s = 2; s <= n; s++) {

        //All combinatons that add up to s via pointer
        auto it = R.begin();
        auto it2 = R.begin();
        it2++;

        while(it != R.end()) {
            //For first iterator
            while(it2 != R.end()) {

                //Check for = s constraint


                it2++;
            }
            it++;
        }

    }

}