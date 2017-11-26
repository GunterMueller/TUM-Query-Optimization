#include "QueryGraph.hpp"
//---------------------------------------------------------------------------

using namespace std;

QueryGraph createGraph(Database& db, const SQLParser::Result& re){
	
	//create Graph object
	QueryGraph query;
	
	//for every relation of the result, we create a node
	for(const auto& relation : re.relations){
		cout << relation.name << "\n";
		auto& table = db.getTable(relation.name);
		//update the statistics
		table.runStats();
		//get the cardinality
		int cardinality = table.getCardinality();
		//now we need to create a node in the graph
		//std::pair npair;
		
		//create a node that has the name of the binding of the relation. also insert carinaliy and relations and constants and create vector for connected edges
		auto npair = make_pair(relation.binding, make_pair(Node(),vector<Edge>()));

		query.insert(npair);									
	}
	
	//let's add selectivities 
	for(const auto& selection : re.selections){
		
			//first we find the node for this relation of the pari
			auto node = query.find(selection.first.relation);
			
			//if there is a node for the relation..
			if(node != query.end()){
				
				 //get the table for the relation
				 auto& table = db.getTable(node->second.first.relation.name);
				 int attr_key = table.findAttribute(selection.first.name);
				 auto& attr = table.getAttribute(attr_key);
				 uint64_t uv = attr.getUniqueValues();
				 auto key = attr.getKey();
				
				//here we can now do the computation we did theoreticaly in the exercises before
				//if htis is a key then  the selectivy is just 1/cardinality 
				if (key) {
					node->second.first.selectivity =  1 / node->second.first.cardinality;
				} else {
					//otherwise  1/domain size
					node->second.first.selectivity = 1 / (double) uv;
				}
					node->second.first.push.push_back(selection);
			}
	}
	
	//let's add the joins
	
	for (const auto& join : re.joinConditions) {
		//the two relations
		auto r1 = query.find(join.first.relation);
		auto r2 = query.find(join.second.relation);
		
		//see if both are in graph
		if (r1 != query.end() && r2 != query.end()) {
			//get both tables
			auto& t1 = db.getTable(r1->second.first.relation.name);
			auto& t2 = db.getTable(r2->second.first.relation.name);
			
			//let's get the needed values for computation 
			int attr_key_r1 = t1.findAttribute(join.first.name);
			auto& attr_r1 = t1.getAttribute(attr_key_r1);
			uint64_t uv_r1 = attr_r1.getUniqueValues();
			auto key_r1 = attr_r1.getKey();
			
			int attr_key_r2 = t2.findAttribute(join.second.name);
			auto& attr_r2 = t2.getAttribute(attr_key_r2);
			uint64_t uv_2 = attr_r2.getUniqueValues();
			auto key_r2 = attr_r2.getKey();
			
			double selectivity = 1.0;
			//if both are keys, then 1 / maximum cardinality 
			if (key_r1 && key_r2) {
				
			}
			//if only r1 is key, then 1/ cardinality size of r1
			else if (key_r1) {
				selectivity = 1/r1->second.first.cardinality;
			} else if (key_r2) {
				selectivity = 1/r2->second.first.cardinality;
			} else{ //if both are not keys, then take 1 / maximum domain size 
				selectivity;
			}
			
			Edge e1{selectivity,r2->second.first};
			Edge e2{selectivity,r1->second.first};

			//let's add the edges
			r1->second.second.push_back(e1);
			r2->second.second.push_back(e2);
	}
	return query;
	}
	//
//---------------------------------------------------------------------------
}