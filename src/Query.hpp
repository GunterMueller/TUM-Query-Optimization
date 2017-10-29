#ifndef H_query
#define H_query

#include <list>
#include <string>
#include <map>

class Query {

    private:
    std::map<std::string,std::string> selectionsMap;
    std::map<std::string,std::string> relationBindingMap;
    std::map<std::string,std::string> conditionsMap;

    public:
    Query(std::map<std::string,std::string> selectionsMap, std::map<std::string,std::string> relationBindingMap, std::map<std::string,std::string> conditionsMap);
    ~Query();

    std::map<std::string,std::string> getSelections();
    std::map<std::string,std::string> getRelationsWithBindings();
    std::map<std::string,std::string> getConditions();

};


#endif