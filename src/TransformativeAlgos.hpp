#ifndef H_TransformativeAlgos
#define H_TransformativeAlgos

#include "JoinTree.hpp"
#include "QueryGraph.hpp"
#include "Utility.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>
#include <numeric>

class TransformativeAlgos {

    public:
    JoinTree exhaustiveTrans2(QueryGraph graph, int numberOfRelations);
    //Replace this by "class"
    void exploreClass(std::vector<int> relSetId);
    void applyTrans(JoinTree theTree);

    std::unordered_map<uint64_t,std::vector<JoinTree>> memo;

};


#endif