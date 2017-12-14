#ifndef H_TransformativeAlgos
#define H_TransformativeAlgos

#include "JoinTree.hpp"
#include "QueryGraph.hpp"

class TransformativeAlgos {

    public:
    JoinTree exhaustiveTrans2(QueryGraph graph);
    void exploreClass();
    void applyTrans(JoinTree theTree);

};


#endif