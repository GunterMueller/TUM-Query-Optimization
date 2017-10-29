#include "Query.hpp"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

Query::Query() {

}

Query::~Query() {

}


void Query::addRelationName(string name) {
    relationNames.push_back(name);
}