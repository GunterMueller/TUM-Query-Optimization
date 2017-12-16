#ifndef H_TransformativeAlgos
#define H_TransformativeAlgos

#include "JoinTree.hpp"
#include "QueryGraph.hpp"
#include "Utility.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>

class TransformativeAlgos {

    public:
    JoinTree exhaustiveTrans2(QueryGraph graph, int numberOfRelations);
    //Replace this by "class"
    void exploreClass(std::unique_ptr<JoinTree> treeClass);
    void applyTrans(JoinTree theTree);

    std::unordered_map<uint64_t,JoinTree&> memo;

};


#endif