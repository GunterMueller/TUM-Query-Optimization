#include "Utility.hpp"

namespace utility {
    void printGraph(QueryGraph& graph) {
        std::cout << "Printing graph" << std::endl;

        for(auto pair : graph) {
            std::cout << "Name " << pair.first << " ";
            std::cout << "Relation: " << pair.second.first.relation_.getName() <<" with ID: " <<
            pair.second.first.relation_.id << std::endl;
            
            for (auto target : pair.second.second) {
                std::cout << "\tEdge to: " << target.connected_to_.relation_.getName() << std::endl;
            }
        }

    }

    void subset(std::vector<int> arr, int size, int left, int index, std::list<int> &l, std::set<std::set<int> >& akku){
        if(left==0){
            std::set<int> newSet;
            newSet.insert(l.begin(),l.end());
            akku.insert(newSet);
            return;
        }
        for(int i=index; i<size;i++){
            l.push_back(arr[i]);
            subset(arr,size,left-1,i+1,l,akku);
            l.pop_back();
        }

    }

    std::set<std::set<int> > subsets(std::vector<int> arr, int size, int left, int index, std::list<int> &l){
        std::set<std::set<int> > result;
        subset(arr, size, left, index, l, result);
        return result;
    }  

    bool setIntersect(std::set<int> A, std::set<int> B) {
        //Mergesort abuse to find intersection
        auto counterA = A.begin();
        auto counterB = B.begin();
        for(;;) {
            if(counterA == A.end() || counterB == B.end())
                return false;
            else if(*counterA == *counterB)
                return true;
            else if(*counterA < *counterB)
                counterA++;
            else if(*counterA > *counterB)
                counterB++;
            else
                //error
                return true;
        }
    }

    void treesFromNodes(std::vector<std::unique_ptr<JoinTree>>& trees,QueryGraph graph){
        for(auto& x : graph) {
            std::cout << "Created tree for: " << x.second.first.relation_.name << std::endl;
            trees.push_back(std::make_unique<JoinTree>(JoinTree(x.second.first)));
        }
    }

}