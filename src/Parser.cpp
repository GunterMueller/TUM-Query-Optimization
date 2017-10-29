#include "Parser.hpp"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

/* TEST QUERY
SELECT s.MatrNr, s.Name, s.Semester
FROM Studenten s, hoeren h, Vorlesungen v, Professoren p
WHERE s.MatrNr=h.MatrNr AND h.VorlNr=v.VorlNr AND
v.gelesenVon=p.PersNr AND p.Name='Curie'
*/

Parser::Parser() 
{

}

Parser::~Parser()
{

}

string trim(string s)
{
    size_t p = s.find(",");
    if(p != string::npos)
        s.erase(p,1);

    p = s.find(" ");
    if(p != string::npos && p == 0)
        s.erase(p,1);

    return s;
}

string trimWhitespace(string s)
{
    size_t p = s.find(" ");
    if(p != string::npos)
        s.erase(p);

    return s;
}

list<string> splitStringToList(string str, string delimiter){

    list<string> result;

    size_t p = str.find(delimiter);
    while(string::npos != p) {
        result.push_back(trim(str.substr(0,p)));
        str.erase(0,p+delimiter.length());
        p = str.find(delimiter);
    }

    result.push_back(trim(str));
    return result;
}

pair<string,string> splitStringToPair(string str, string delimiter){
    pair<string,string> result;
    size_t p = str.find(delimiter);
    result = make_pair(str.substr(0,p),str.substr(p+1,str.length()));
    return result;
}

Query Parser::parse(std::string query) 
{
    //First naive implementation
    //TODO: Restructure (move code to methods,etc.)

    
    //SELECT
    string keyword = "SELECT ";
    //Find SELECT and delete everything prior together with the keyword
    size_t pos = 0;
    pos = query.find(keyword);
    query.erase(0, pos + keyword.length());

    //Find FROM and save everything in front of the keyword as the selection substring
    //Then erase
    keyword = "FROM ";
    pos = query.find(keyword);

    string selectionString = query.substr(0,pos);
    query.erase(0,pos + keyword.length());

    //Find WHERE and save everything in front of the keyword as the substring of the FROM clause
    keyword = "WHERE";
    pos = query.find(keyword);

    string fromString = query.substr(0,pos);
    query.erase(0,pos + keyword.length());

    cout << "SELECTION: " + selectionString + "\n";
    cout << "FROM: " + fromString + "\n";
    cout << "WHERE: " + query + "\n";

    // Further parse substrings
    list<string> selectionList = splitStringToList(selectionString,",");
    list<string> fromList = splitStringToList(fromString,",");
    list<string> whereConditions= splitStringToList(query,"AND");

    //From strings to pairs/...
    map<string,string> relationBindingMap;
    cout << "Creating binding/relation map" << "\n";
    list<string>::iterator it = fromList.begin();
    size_t whitespacePos = 0;
    while(it != fromList.end()) {
        string str = *it;

        //Everything to lower case
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);

        //Split at blanks
        whitespacePos = str.find(" ");
        string relation = str.substr(0,whitespacePos);
        str.erase(0,whitespacePos+1);

        //Add binding/relation pair to list
        cout << "Adding: '" << str << "' \\ '" << relation << "'\n";
        relationBindingMap.insert(pair<string,string>(trimWhitespace(str),trimWhitespace(relation)));

        //Next
        it++;
    }
    cout << "Done: Creating binding/relation map" << "\n";

    //Selection attribute/binding map
    cout << "Creating selection map" << "\n";
    map<string,string> selectionsMap;
    it = selectionList.begin();
    size_t dotPos = 0;
    while(it != selectionList.end()) {
        string str = *it;
        //Split at .
        dotPos = str.find(".");
        string binding = str.substr(0,dotPos);
        cout << binding << "\n";
        str.erase(0,dotPos+1);
        cout << str << "\n";
        //Selection is case insensitive, transform to lowercase for consistency
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        //Also trim whitespace, just in case, but dont use our modified method
        str = trimWhitespace(str);
        selectionsMap.insert(pair<string,string>(str,binding));

        it++;
    }
    cout << "Done: Creating selection map" << "\n";

    //WHERE conditions
    cout << "Creating condition map" << "\n";
    map<string,string> conditionMap;
    it = whereConditions.begin();
    while(it != whereConditions.end()) {
        pair<string,string> comparison = splitStringToPair(*it,"=");
        cout << "LHS: " << comparison.first << " RHS: " << comparison.second << "\n";
        conditionMap.insert(comparison);
        it++;
    }
    cout << "Done: Creating condition map" << "\n";

    //Semantic checks ------------------------------------------
    Database db;
    db.open("data/uni");
    //start with FROM, check table existence
    map<string,string>::iterator mapIt = relationBindingMap.begin();
    cout << "Checking table existence" << "\n";
    while(mapIt != relationBindingMap.end()) {
        if(!db.hasTable(mapIt->second)){
            //Throw error
            //For now just:
            cout << "The table " << mapIt->second << "was not found!" << "\n";
        }
        mapIt++;
    }
    cout << "Done: Checking table existence" << "\n";

    //Using binding mapping, check for attributes
    mapIt = selectionsMap.begin();
    cout << "Checking selection attribute existence" << "\n";
    while(mapIt != selectionsMap.end()) {

        cout << mapIt->first << "\n";
        if(db.hasTable(relationBindingMap[mapIt->second])) {
            Table& testTable = db.getTable(relationBindingMap[mapIt->second]);
        
            if(testTable.findAttribute(mapIt->first) == -1){
                //Not found in table, throw error
                //For now just:
                cout << "The attribute " << mapIt->first << " was not found in table  "
                << relationBindingMap[mapIt->second] << "!" << "\n";
            }
        } else {
            cout << "Table: " << relationBindingMap[mapIt->second] << " not found!";
        }

        mapIt++;
    }
    cout << "Done: Checking selection attribute existence" << "\n";

    //Check condition attributes
    mapIt = conditionMap.begin();
    cout << "Checking condition attribute existence" << "\n";
    while(mapIt != conditionMap.end()) {
        string LHS = mapIt->first;
        string RHS = mapIt->second;

        //Binding/Attribute
        pair<string,string> lhsBindingAttribute = splitStringToPair(LHS,".");
        pair<string,string> rhsBindingAttribute = splitStringToPair(RHS,".");

        //Resolve binding and check
        if(!db.hasTable(relationBindingMap[lhsBindingAttribute.first])){
            //Throw error
            //For now
            cout << "LHS: Table " << relationBindingMap[lhsBindingAttribute.first] << " not found!\n";
            
            //Exclude from check if LHS==RHS, result of split if comparison with constant
        } else if(rhsBindingAttribute.first != rhsBindingAttribute.second && !db.hasTable(relationBindingMap[rhsBindingAttribute.first])){
            //Throw error
            //For now
            cout << "RHS: Table " << relationBindingMap[rhsBindingAttribute.first] << " not found!\n";
        }

        mapIt++;
    }
    cout << "Done: Checking condition attribute existence" << "\n";

    //Finally, construct query object
    Query queryObj(selectionsMap,relationBindingMap,conditionMap);
    return queryObj;
}