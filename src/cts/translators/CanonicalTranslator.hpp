#ifndef H_CanonicalTranslator
#define H_CanonicalTranslator
//---------------------------------------------------------------------------
#include <string>
#include <iostream>
#include "cts/parser/SQLParser.hpp"
//---------------------------------------------------------------------------
/// The basic canonical translator (with pushed-down constant predicates)

class CanonicalTranslator 
{
    public:

    private:
    SQLParser::Result parserResult;

    public:
    //Constructor
    CanonicalTranslator(SQLParser::Result res);
    //Destructor
    ~CanonicalTranslator();

};
//---------------------------------------------------------------------------
#endif