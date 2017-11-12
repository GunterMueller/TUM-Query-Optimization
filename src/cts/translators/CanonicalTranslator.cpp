#include "CanonicalTranslator.hpp"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
CanonicalTranslator::CanonicalTranslator(SQLParser::Result res, Database* db) : parserResult(res), db(db)
    // Constructor
{

}
//---------------------------------------------------------------------------
CanonicalTranslator::~CanonicalTranslator()
   // Destructor
{
}

void CanonicalTranslator::addOpToQueue(CanonicalTranslator::Type type, std::unique_ptr<Operator> op) {
    operatorVector.push_back(make_pair(type,move(op)));
}

unique_ptr<Operator> CanonicalTranslator::translate() {

    auto relations = parserResult.relations;
	auto joinConditions = parserResult.joinConditions;
	auto selections = parserResult.selections;

    for(auto it : relations) {
		//Loop over all relations of the query and look for attr=const selection
		//We care for them first, to achieve "pushed-down predicates"
		//Get the table
		Table& rel = db->getTable(it.name);
		//The scan
		unique_ptr<Tablescan> scan(new Tablescan(rel));

		auto hit = std::find_if(selections.begin(), selections.end(), [it](const std::pair<SQLParser::RelationAttribute, SQLParser::Constant>& element) {
			return element.first.relation == it.binding;
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
			cout << "Selecting for: " << hit->first.name << " " << c.getString() << "\n";
			const Register* attr=scan->getOutput(hit->first.name);
			unique_ptr<Selection> select( new Selection(move(scan),attr,&c));

			//Put relation + selection in our "plan" (queue)
			addOpToQueue(Type::Selection, move(select));

		} else {
			// There is no predicate to be pushed down in our current translation scheme (canonical plus constant selections)
			// Just get what we need for the cross products and add to plan
			// Get the table/scan
			addOpToQueue(Type::Scan, move(scan));
		}
	}


	//Now for canonical just reduce our queue via crossproduct
	//Take first relation out and then do one cross product on top of the other
	auto firstOp = move(operatorVector.back());
	operatorVector.pop_back();

	unique_ptr<CrossProduct> cp(new CrossProduct(move(firstOp.second),move(operatorVector.back().second)));
	operatorVector.pop_back();

	cout << "Constructing Cross Products \n";
	while(!operatorVector.empty()) {
		unique_ptr<Operator> secondRel = move(operatorVector.back().second);
		operatorVector.pop_back();

		unique_ptr<CrossProduct> res(new CrossProduct(move(cp),move(secondRel)));
		cp = move(res);
	}

	operatorVector.push_back(make_pair(Type::CrossProduct,move(cp)));

	//Other predicates, here: join conditions
	//cout << "Next: Other predicates from the WHERE clause \n";
	
	//std::vector<std::pair<SQLParser::RelationAttribute, SQLParser::RelationAttribute>>::iterator jcIterator = joinConditions.begin();

	//Resolve LHS of the predicate
	//cout << "LHS: " << jcIterator->first.relation << "." << jcIterator->first.name << "\n";
	//auto matchIt = std::find_if(relations.begin(), relations.end(), [jcIterator](const SQLParser::Relation& element) {
		//return element.binding == jcIterator->first.relation;
	//});
	//cout << jcIterator->first.relation << " resolved to: " << matchIt->name << "\n";

	/**
	Table& t=db.getTable("studenten");
	unique_ptr<Tablescan> scan(new Tablescan(t));
	const Register* attr=scan->getOutput("matrnr");
	**/

	//Resolve RHS of the predicate
	//cout << "RHS: " << jcIterator->second.relation << "." << jcIterator->second.name << "\n";
	//matchIt = std::find_if(relations.begin(), relations.end(), [jcIterator](const SQLParser::Relation& element) {
	//	return element.binding == jcIterator->second.relation;
	//});
	//cout << jcIterator->second.relation << " resolved to: " << matchIt->name << "\n";

	/**
	Table& t2=db.getTable("hoeren");
	unique_ptr<Tablescan> scan2(new Tablescan(t2));
	const Register* attr2=scan2->getOutput("matrnr");
	**/

	//Selection
	//unique_ptr<Selection> select(new Selection(move(cp),attr,&two));












	/**
    auto firstOp = move(operatorVector.back());
    operatorVector.pop_back();

    switch(firstOp.first) {
        case Type::Selection:

            break;
        case Type::Scan:

            break;
        case Type::Projection:

            break;
    }
	**/



    return move(operatorVector.back().second);
}