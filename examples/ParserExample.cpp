#include "Database.hpp"
#include "Parser.hpp"
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

    parser.parse("SELECT s.MatrNr, s.Name, s.Semester FROM Studenten s, hoeren h, Vorlesungen v, Professoren p WHERE s.MatrNr=h.MatrNr AND h.VorlNr=v.VorlNr AND v.gelesenVon=p.PersNr AND p.Name='Curie'");
}