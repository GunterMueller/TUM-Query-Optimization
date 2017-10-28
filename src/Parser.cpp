#include "Parser.hpp"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

/* TEST QUERY
SELECT s.MatrNr, s.Name, s.Semester
FROM Studenten s, hoeren h, Vorlesungen v, Professoren p
WHERE s.MatrNr=h.MatrNr AND h.VorlNr=v.VorlNr AND
v.gelesenVon=p.PersNr AND p.Name=’Curie’
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

void Parser::parse(std::string query) 
{
    //First naive implementation

    
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

    list<string>::iterator it = selectionList.begin();

    while(it != selectionList.end()){
        cout << *it << "\n";
        it++;
    }

}