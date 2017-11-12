#ifndef H_CanonicalTranslator
#define H_CanonicalTranslator
//---------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <iostream>
#include <queue>
#include "cts/parser/SQLParser.hpp"
#include "operator/Projection.hpp"
#include "operator/Selection.hpp"
#include "operator/Tablescan.hpp"
#include "Database.hpp"
//---------------------------------------------------------------------------
/// The basic canonical translator (with pushed-down constant predicates)

class CanonicalTranslator 
{
    public:
    /// Possible operators
    enum class Type : unsigned { Selection, Scan, Projection};
    
    private:
    SQLParser::Result parserResult;
    std::vector<std::pair<CanonicalTranslator::Type,std::unique_ptr<Operator>>> operatorVector;
    Database* db;

    public:
    //Constructor
    CanonicalTranslator(SQLParser::Result res, Database* db);
    //Destructor
    ~CanonicalTranslator();
    
    //Add to op queue
    void addOpToQueue(CanonicalTranslator::Type type, std::unique_ptr<Operator>);
    
    //Translation method
    std::unique_ptr<Operator> translate();
};
//---------------------------------------------------------------------------
#endif