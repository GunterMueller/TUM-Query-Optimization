#ifndef H_CanonicalTranslator
#define H_CanonicalTranslator
//---------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <iostream>
#include <queue>
#include <map>
#include "cts/parser/SQLParser.hpp"
#include "operator/Projection.hpp"
#include "operator/Selection.hpp"
#include "operator/Tablescan.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/Printer.hpp"
#include "Database.hpp"
//---------------------------------------------------------------------------
/// The basic canonical translator (with pushed-down constant predicates)

class CanonicalTranslator 
{
    public:
    /// Possible operators
    enum class Type : unsigned { Selection, Scan, Projection, CrossProduct};
    
    private:
    SQLParser::Result parserResult;
    std::vector<std::pair<CanonicalTranslator::Type,std::unique_ptr<Operator>>> operatorVector;
    Database* db;
    std::map<std::string,const Register*> registerMap;

    public:
    //Constructor
    CanonicalTranslator(SQLParser::Result res, Database* db);
    //Destructor
    ~CanonicalTranslator();
    
    //Add to op queue
    void addOpToQueue(CanonicalTranslator::Type type, std::unique_ptr<Operator>);
    
    //Translation method
    std::unique_ptr<Operator> translate();

    private:
    std::unique_ptr<Operator> filterWhere(std::unique_ptr<Operator> startOp, std::pair<SQLParser::RelationAttribute,SQLParser::RelationAttribute> joinCond,std::vector<SQLParser::Relation> relations);
};
//---------------------------------------------------------------------------
#endif