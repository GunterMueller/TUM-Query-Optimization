#ifndef H_query
#define H_query

#include <list>
#include <string>



class Query {

    std::list<std::string> relationNames;

    public:
    Query();
    ~Query();
    

    void addRelationName(std::string name);
};


#endif