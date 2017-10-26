#ifndef H_Parser
#define H_Parser
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------

class Parser 
{

    public:
    //Constructor
    Parser();
    //Destructor
    ~Parser();
    
    //Parse a string
    void parse(const std::string& query);



};

#endif