#include <iostream>
#include <sstream>
#include "Database.hpp"
#include "QueryGraph.hpp"
#include "GreedyOperatorOrdering.hpp"
#include "cts/semana/SemanticAnalysis.hpp"
#include "DPAlgos.hpp"


std::string getQuery(std::ifstream& in) {
  std::ostringstream buf;
  std::string line;
  while (true) {
    std::getline(in, line);
    buf << line;
    if (!in.good()) {
      break;
    }
    buf << std::endl;
  }
  return buf.str();
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <db> <query file> " << std::endl;
    return 1;
  }

  Database db;
  db.open(argv[1]);
  std::ifstream input(argv[2]);
  std::string query = getQuery(input);
  input.close();

  SQLLexer lexer(query);
  SQLParser parser(lexer);

  try {
    parser.parse();
  } catch (std::runtime_error& e) {
    std::cout << "exception: " << e.what() << std::endl;
    return 1;
  }

  SQLParser::Result res = parser.getParserResult();
  try {
    SemanticAnalysis semana(db);
    semana.analyze(res);
  } catch (SemanticAnalysis::SemanticError& ex) {
    std::cerr << "exception: " << ex.what() << std::endl;
    return 1;
  }

  auto graph = make_query_graph(db, res);
  //auto jt = run(graph);
  
  DPAlgos algo;
  
  auto mapJT = algo.DPsize(graph);

  for(auto k : mapJT) {
   std::cout << "Index: " << k.first << " JoinTree Object" << endl;;
  }

  return 0;
}