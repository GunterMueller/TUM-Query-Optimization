#ifndef H_Parser
#define H_Parser
//---------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <istream>
#include <list>
//---------------------------------------------------------------------------

class Parser 
{
    public:
    //Constructor
    Parser();
    //Destructor
    ~Parser();
    
    //Parse a string
    void parse(std::string query);
};

#endif