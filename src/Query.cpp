#include "Query.hpp"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

map<string,string> selectionsMap;
map<string,string> relationBindingMap;
map<string,string> conditionsMap;

Query::Query(map<string,string> selectionsMap,map<string,string> relationBindingMap,map<string,string> conditionsMap)
:selectionsMap(selectionsMap),relationBindingMap(relationBindingMap),conditionsMap(conditionsMap)
{ 

}

Query::~Query() {

}

std::map<std::string,std::string> Query::getSelections() {
    return selectionsMap;
}

std::map<std::string,std::string> Query::getRelationsWithBindings() {
    return relationBindingMap;
}

std::map<std::string,std::string> Query::getConditions() {
    return conditionsMap;
}