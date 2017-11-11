#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include "Database.hpp"
#include "cts/parser/SQLLexer.hpp"
#include "cts/parser/SQLParser.hpp"
#include "cts/semana/SemanticAnalysis.hpp"

#include "operator/Tablescan.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/Printer.hpp"
#include "operator/Selection.hpp"

using namespace std;

using Token = SQLLexer::Token;

string getQuery(ifstream& in){
	ostringstream buf;
	string line;
	while (true){
		getline(in, line);
		buf << line;
		if (!in.good()){
			break;
		}
		buf << endl;
	}
	return buf.str();
}


void displayParserResult(SQLParser::Result& result){
	cout << "projections: " << endl;
	for (auto& p: result.projections){
		cout << "  " << p.getName() << " " << endl;
	}

	cout << "relations: " << endl;
	for (auto& r: result.relations){
		cout << "  " << r.getName() << endl;
	}

	cout << "join conditions: ";
	for (auto& join: result.joinConditions){
		cout << "  " << join.first.getName() << "=" << join.second.getName();
	}
	cout << endl;

	cout << "selections: ";
	for (auto& sel: result.selections){
		cout << "  " << sel.first.getName() << "=" << sel.second.value << endl;
	}
	cout << endl;

}

int main(int argc, char* argv[]){
	if (argc < 3){
		cerr << "usage: "<<argv[0]<<" <db> <query file> "<< endl;
		return 1;
	}

	Database db;
	db.open(argv[1]);
	ifstream input(argv[2]);
	string query = getQuery(input);
	input.close();

	SQLLexer lexer(query);
	SQLParser parser(lexer);

	try{
		parser.parse();
	}
	catch (runtime_error& e){
		cout << "exception: "<<e.what() << endl;
		return 1;
	}

	SQLParser::Result res = parser.getParserResult();
	displayParserResult(res);
	try {
		SemanticAnalysis semana(db);
		semana.analyze(res);
	}
	catch (SemanticAnalysis::SemanticError& ex){
		cerr << "exception: "<<ex.what() << endl;
		return 1;
	}
	cout << "Semantic Analysis successful" << endl;

	//Now construct the execution plan from the parser result
	//Canonical (un-optimized) translation, but with attr = const predicated pushed down


	//1. Cross-product of entries in the from clause
	cout << "\n++++++++Phase 1 of canonical translation+++++++++\n";
	std::vector<SQLParser::Relation>& relations = res.relations;
	//Selections
	std::vector<std::pair<SQLParser::RelationAttribute, SQLParser::Constant>> sel = res.selections;

	std::vector<SQLParser::Relation>::iterator it = relations.begin();

	//Our queue of relations to cross product
	std::vector<unique_ptr<Operator>> plan;

	while(it != relations.end()) {
		//Loop over all relations of the query and look for attr=const selection
		//We care for them first, to achieve "pushed-down predicates"
		
		auto hit = std::find_if(sel.begin(), sel.end(), [it](const std::pair<SQLParser::RelationAttribute, SQLParser::Constant>& element) {
			return element.first.relation == it->binding;
		});

		if(hit != sel.end()) {
			// Do selection now
			cout << "Match: " << hit->first.relation << "." << hit->first.name << "=" << hit->second.value << "\n";
			
			//Get the table
			Table& rel=db.getTable(it->name);
			unique_ptr<Tablescan> scan(new Tablescan(rel));
			//The constant
			Register c;
			//Set depending on type:
			switch(hit->second.type) {
				case Attribute::Type::String:
					c.setString(hit->second.value);
					break;
				case Attribute::Type::Int:
					c.setInt(stoi(hit->second.value));
					break;
				case Attribute::Type::Double:
					c.setDouble(stod(hit->second.value));
					break;
				case Attribute::Type::Bool:
					std::transform(hit->second.value.begin(), hit->second.value.end(), hit->second.value.begin(), ::tolower);
    				std::istringstream is(hit->second.value);
    				bool b;
    				is >> std::boolalpha >> b;
					c.setBool(b);
					break;
			}
			//Now select
			cout << hit->first.name << " " << c.getString() << "\n";
			const Register* attr=scan->getOutput(hit->first.name);
			unique_ptr<Selection> select( new Selection(move(scan),attr,&c));


			//Put selected relation in our "plan" (in this canonical case just a vector)
			plan.push_back(move(select));
		}
		
		it++;
	}

	
	Printer out(move(plan.back()));
	cout << "\nShowing results: \n";
	out.open();
	while (out.next());
	out.close();
	
	



	return 0;

}