#ifndef H_TransformativeAlgos
#define H_TransformativeAlgos

#include "JoinTree.hpp"
#include "QueryGraph.hpp"

class TransformativeAlgos {

    public:
    JoinTree exhaustiveTrans2(QueryGraph graph);
    //Replace this by "class"
    void exploreClass(std::unique_ptr<JoinTree> treeClass);
    void applyTrans(JoinTree theTree);

};


#endif