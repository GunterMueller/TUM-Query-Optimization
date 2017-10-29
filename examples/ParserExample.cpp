#include "Database.hpp"
#include "Parser.hpp"
#include "Query.hpp"
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

int main ()
{
    cout << "Parser Test \n";
    Database db;
    db.open("data/uni");

    Parser parser;

    //Parse a query string to a query object, here the example from the slides
    Query q = parser.parse("SELECT s.MatrNr, s.Name, s.Semester FROM Studenten s, hoeren h, Vorlesungen v, Professoren p WHERE s.MatrNr=h.MatrNr AND h.VorlNr=v.VorlNr AND v.gelesenVon=p.PersNr AND p.Name='Curie'");

    

    //Print query content
    map<string,string> select = q.getSelections();
    map<string,string> relations = q.getRelationsWithBindings();
    map<string,string> conditions = q.getConditions();
    cout << "\n" << "RESULT \n";
    map<string,string>::iterator it = select.begin();
    while(it != select.end()) {
        cout << "Selection: " << it->first << " of " << it->second << "\n";
        it++;
    }

    it = relations.begin();
    while(it != relations.end()) {
        cout << "Relation: " << it->first << "<-" << it->second << "\n";
        it++;
    }

    it = conditions.begin();
    while(it != conditions.end()) {
        cout << "Condition: " << it->first << "=" << it->second << "\n";
        it++;
    }

}