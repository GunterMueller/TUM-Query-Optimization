#include "DPAlgos.hpp"

using namespace std;

//Joins two trees
JoinTree DPAlgos::CreateJoinTree(JoinTree&& left, JoinTree&& right) {
    //We currently just use what is already implemented from previous exercises
    //So no min. over join implementations
    //Call the constructor
    return JoinTree(std::move(left),std::move(right));
}

//In: A set of relations (we re-use the query graph that was generated, just query graph node objects instead of SQLParser::relation)
//Out: Optimal bushy join tree
//Note that this is not optimized in any way
map<string,JoinTree> DPAlgos::DPsize(QueryGraph graph) {

    //Get set of nodes (JoinTree is only a single relation here)
    //We index our relations by integers
    vector<pair<int,JoinTree>> R;

    //DP table (we combine indices to strings)
    map<string,JoinTree> B;
    int k = 1;
    for(auto kv : graph) {
        R.push_back(make_pair(k,JoinTree(kv.second.first)));
        B.insert(make_pair(to_string(k),JoinTree(kv.second.first)));
    }

    int n = R.size();

    //Get subsets
    vector<int> start(n);
    for (int i = 1; i <= n; i++)
    {
        start[i] = i;
    }

    //Enumerate subsets that sum up to s
    for(int s = 2; s <= n; s++) {

        int first = s / 2;
        int second = s - first;

        list<int> lt;   
        set<set<int> > firstSet = utility::subsets(start,n,first,0,lt);
        list<int> lt2;   
        set<set<int> > secondSet = utility::subsets(start,n,second,0,lt2);
        
        for(auto s1 : firstSet) {
            for(auto s2: secondSet) {

                //If intersection != empty set continue#
                if(utility::setIntersect(s1,s2)){
                    continue;
                } else {

                    string ind;
                    string ind2;
                    for(int x : s1) {
                        ind += to_string(x);
                    }
                    
                    
                    for(int y : s2) {
                        ind2 += to_string(y);
                    }

                    

                    if(B.find(ind) == B.end() || B.find(ind2) == B.end()) {
                        continue;
                    } else {
                        JoinTree P = CreateJoinTree(move(B[ind]),move(B[ind2]));

                        if(B.find(ind+ind2) == B.end()) {

                        } else if(B[ind+ind2].cost(graph) > P.cost(graph)) {
                            B.insert(make_pair(ind+ind2,P));
                        }
                    }

                }

            }
        }
        

    }

    return B;

}