#include "DPAlgos.hpp"

//Joins two trees
static JoinTree CreateJoinTree(JoinTree&& left, JoinTree&& right) {
    //We currently just use what is already implemented from previous exercises
    //So no min. over join implementations
    //Call the constructor
    return JoinTree(std::move(left),std::move(right));
}

//In: A set of relations (here we start from parser result)
//Out: Optimal bushy join tree
static void DPsize(SQLParser::Result res) {

    //Get set of relations
    auto relations = res.relations;

    for (auto& rel : relations) {
        //Set entry in DP table
    }

}