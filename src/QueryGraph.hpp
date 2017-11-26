#ifndef _QueryGraph_hpp
#define _QueryGraph_hpp

#include <vector>
#include <string>
#include <unordered_map>
#include "Database.hpp"
#include "cts/parser/SQLParser.hpp"

struct Node{
    //Estimated result cardinality
    int cardinality;
    //Selectivity
    double selectivity;
    //The underlying relation
    SQLParser::Relation relation;
    //List of predicates that can be pushed down to the relation
    std::vector<std::pair<SQLParser::RelationAttribute, SQLParser::Constant>> push;
};

struct Edge{
    //The estimated selectivity
    double selectivity;
    //Connection
    Node& connection;
};

using QueryGraph = std::unordered_map<std::string, std::pair<Node, std::vector<Edge>>>;
    
QueryGraph createGraph(Database& db, const SQLParser::Result& res);    

namespace std {
  template <> struct hash<Node>
  {
    size_t operator()(const Node & x) const
    {
      return hash<std::string>()(x.relation.binding);
    }
  };
}	
	
#endif
    

    
