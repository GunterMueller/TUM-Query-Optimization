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
			// Get the table
			addOpToQueue(Type::Scan, move(scan));
		}
	}

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

    return move(firstOp.second);
}