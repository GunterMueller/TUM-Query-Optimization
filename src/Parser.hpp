#ifndef H_Parser
#define H_Parser
//---------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <istream>
#include <list>
#include <map>
#include <algorithm>
#include <utility>
#include "Query.hpp"
#include "Database.hpp"
//---------------------------------------------------------------------------

class Parser 
{
    public:
    //Constructor
    Parser();
    //Destructor
    ~Parser();
    
    //Parse a string
    Query parse(std::string query);
};

#endif