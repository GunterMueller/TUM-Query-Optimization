#include "QueryGraph.hpp"


QueryGraph make_query_graph(Database& db, const SQLParser::Result& res) {
  QueryGraph graph;

  for (const auto& relation : res.relations) {
    auto& table = db.getTable(relation.name);
    table.runStats();
    uint64_t cardinality = table.getCardinality();
    graph.insert(std::make_pair(
        relation.binding,
        std::make_pair(
            QueryGraphNode{relation, cardinality, 1.0,
                           std::vector<std::pair<SQLParser::RelationAttribute,
                                                 SQLParser::Constant>>()},
            std::vector<QueryGraphEdge>())));
  }

  for (const auto& selection : res.selections) {
    auto node = graph.find(selection.first.relation);
    if (node != graph.end()) {
      auto& table = db.getTable(node->second.first.relation_.name);
      int attr_key = table.findAttribute(selection.first.name);
      auto& attr = table.getAttribute(attr_key);
      uint64_t uv = attr.getUniqueValues();
      auto key = attr.getKey();

      if (key) {
        node->second.first.selectivity_selections_ *=
            1.0 / (double)node->second.first.cardinality_;
      } else {
        node->second.first.selectivity_selections_ *= 1.0 / (double)uv;
      }
      node->second.first.pushed_down_.push_back(selection);
    }
  }

  for (const auto& join : res.joinConditions) {
    auto p1 = graph.find(join.first.relation);
    auto p2 = graph.find(join.second.relation);

    if (p1 != graph.end() && p2 != graph.end()) {
      auto& t1 = db.getTable(p1->second.first.relation_.name);
      auto& t2 = db.getTable(p2->second.first.relation_.name);

      int attr_key_p1 = t1.findAttribute(join.first.name);
      auto& attr_p1 = t1.getAttribute(attr_key_p1);
      uint64_t uv_p1 = attr_p1.getUniqueValues();
      auto key_p1 = attr_p1.getKey();

      int attr_key_p2 = t2.findAttribute(join.second.name);
      auto& attr_p2 = t2.getAttribute(attr_key_p2);
      uint64_t uv_p2 = attr_p2.getUniqueValues();
      auto key_p2 = attr_p2.getKey();

      double selectivity = 1.0;
      if (key_p1 && key_p2) {
        selectivity /=
            (p1->second.first.cardinality_ > p2->second.first.cardinality_)
                ? (double)p1->second.first.cardinality_
                : (double)p2->second.first.cardinality_;
      } else if (key_p1) {
        selectivity /= p1->second.first.cardinality_;
      } else if (key_p2) {
        selectivity /= p2->second.first.cardinality_;
      } else {
        selectivity /= (uv_p1 > uv_p2) ? (double)uv_p1 : (double)uv_p2;
      }

      QueryGraphEdge e1{p2->second.first, selectivity};
      QueryGraphEdge e2{p1->second.first, selectivity};

      p1->second.second.push_back(e1);
      p2->second.second.push_back(e2);
    }
  }

  return graph;
}