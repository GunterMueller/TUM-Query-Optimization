#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include "Database.hpp"
#include "cts/parser/SQLLexer.hpp"
#include "cts/parser/SQLParser.hpp"
#include "cts/semana/SemanticAnalysis.hpp"
#include "cts/translators/CanonicalTranslator.hpp"

#include "operator/Tablescan.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/Printer.hpp"
#include "operator/Selection.hpp"
#include "operator/Projection.hpp"

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


	//TODO: Plug in canonical translator here
	CanonicalTranslator translator(res, &db);
	//Start translation, returns pointer to final operator
	unique_ptr<Operator> translationResult = translator.translate();

	Printer out(move(translationResult));
   	out.open();
   	while (out.next());
   	out.close();

	/**
	Table& studi=db.getTable("studenten");
   	Table& hoeren=db.getTable("hoeren");

	unique_ptr<Tablescan> scanStudi(new Tablescan(studi));
   	unique_ptr<Tablescan> scanHoeren(new Tablescan(hoeren));

	const Register* name=scanStudi->getOutput("name");
   	const Register* studimatrnr=scanStudi->getOutput("matrnr");
   	const Register* hoerennr=scanHoeren->getOutput("matrnr");
   	
	
	Register c;
	c.setString("Schopenhauer");
	unique_ptr<Selection> firstSel(new Selection(move(scanStudi),name,&c));
	unique_ptr<CrossProduct> cp(new CrossProduct(move(firstSel),move(scanHoeren)));
	unique_ptr<Selection> select(new Selection(move(cp),studimatrnr,hoerennr));
  	unique_ptr<Projection> project(new Projection(move(select),{name}));

	Printer out(move(project));

   	out.open();
   	while (out.next());
   	out.close();
	**/

	
	//Now construct the execution plan from the parser result
	//Canonical (un-optimized) translation, but with attr = const predicated pushed down

	/**
	cout << "\n++++++++Canonical translation+++++++++\n";
	auto relations = res.relations;
	auto joinConditions = res.joinConditions;
	auto selections = res.selections;

	//Our queue of relations to cross product
	std::queue<unique_ptr<Operator>> plan;

	cout << "\n+++++++++Applying matching selection with attr=const form \n";
	
	for(auto it : relations) {
		//Loop over all relations of the query and look for attr=const selection
		//We care for them first, to achieve "pushed-down predicates"
		
		//Get the table
		Table& rel = db.getTable(it.name);
		//The scan
		unique_ptr<Tablescan> scan(new Tablescan(rel));


		auto hit = std::find_if(selections.begin(), selections.end(), [it](const std::pair<SQLParser::RelationAttribute, SQLParser::Constant>& element) {
			return element.first.relation == it->binding;
		});

		if(hit != selections.end()) {
			// Do selection now
			cout << "Match: " << hit->first.relation << "." << hit->first.name << "=" << hit->second.value << "\n";
			
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
			plan.push(move(select));
		} else {
			// There is no predicate to be pushed down in our current translation scheme (canonical plus constant selections)
			// Just get what we need for the cross products and add to plan
			//Get the table
			Table& rel=db.getTable(it->name);
			unique_ptr<Tablescan> scan(new Tablescan(rel));
			plan.push(move(scan));
		}
	}

	//Now for canonical just reduce our queue via crossproduct
	//Take first relation out and then do one cross product on top of the other
	unique_ptr<Operator> firstOp = move(plan.front());
	plan.pop();

	unique_ptr<CrossProduct> cp(new CrossProduct(move(firstOp),move(plan.front())));
	plan.pop();

	cout << "Constructing Cross Products \n";
	while(!plan.empty()) {
		unique_ptr<Operator> secondRel = move(plan.front());
		plan.pop();

		unique_ptr<CrossProduct> res(new CrossProduct(move(cp),move(secondRel)));
		cp = move(res);
	}

	//Other predicates, here: join conditions
	cout << "Next: Other predicates from the WHERE clause \n";
	
	std::vector<std::pair<SQLParser::RelationAttribute, SQLParser::RelationAttribute>>::iterator jcIterator = joinConditions.begin();

	//Resolve LHS of the predicate
	//cout << "LHS: " << jcIterator->first.relation << "." << jcIterator->first.name << "\n";
	//auto matchIt = std::find_if(relations.begin(), relations.end(), [jcIterator](const SQLParser::Relation& element) {
		//return element.binding == jcIterator->first.relation;
	//});
	//cout << jcIterator->first.relation << " resolved to: " << matchIt->name << "\n";

	Table& t=db.getTable("studenten");
	unique_ptr<Tablescan> scan(new Tablescan(t));
	const Register* attr=scan->getOutput("matrnr");

	//Resolve RHS of the predicate
	//cout << "RHS: " << jcIterator->second.relation << "." << jcIterator->second.name << "\n";
	//matchIt = std::find_if(relations.begin(), relations.end(), [jcIterator](const SQLParser::Relation& element) {
	//	return element.binding == jcIterator->second.relation;
	//});
	//cout << jcIterator->second.relation << " resolved to: " << matchIt->name << "\n";

	Table& t2=db.getTable("hoeren");
	unique_ptr<Tablescan> scan2(new Tablescan(t2));
	const Register* attr2=scan2->getOutput("matrnr");

	//Selection
	//unique_ptr<Selection> select(new Selection(move(cp),attr,&two));
	
	//TODO: fix selection
	

	
	Printer out(move(cp));
	cout << "\nShowing results: \n";
	out.open();
	while (out.next());
	out.close();
	**/
	return 0;
}