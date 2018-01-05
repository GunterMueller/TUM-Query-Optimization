#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Database.hpp"
#include "QueryGraph.hpp"
#include "GreedyOperatorOrdering.hpp"
#include "cts/semana/SemanticAnalysis.hpp"
#include "DPAlgos.hpp"
#include "TransformativeAlgos.hpp"
#include "Utility.hpp"
#include "QuickPick.hpp"

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

  //Enumerate relations (this could be done earlier to save a loop)
  //okay here for our purposes
  int i = 1;
  for (auto& r : res.relations) {
    r.setId(i);
    i++;
  }

  auto graph = make_query_graph(db, res);
  //Something to hold all our join trees, we might generate, etc.
  std::vector<JoinTree> trees;
  //Transformative Algo
  TransformativeAlgos algos;
  //QuickPick quick(trees,graph);
  //Show graph
  utility::printGraph(graph);

  //Apply Tranformation
  //JoinTree tree = algos.exhaustiveTrans2(graph,res.relations.size());

  //Ex9
  //Run quick pick 100 times
  std::srand(std::time(nullptr)); 
  std::ofstream file;
  const char* extension = ".txt";
  file.open(strcat(argv[2],extension));
  
  for(int i = 0; i < 100; i++){
    QueryGraph graph2 = graph;
    utility::treesFromNodes(trees,graph2);
    QuickPick quick(trees,graph2);
    std::unique_ptr<JoinTree> t = quick.QP();
    double cost = t.get()->cost(graph2);
    std::cout << "Cost of result tree: " << cost << std::endl;
    file << cost << std::endl;
  }
  file.close();
  return 0;
}